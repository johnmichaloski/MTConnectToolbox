//
// MicroInterpreter.cpp
// 
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

/**

http://www.drdobbs.com/article/print?articleID=184401692&dept_url=/cpp/
*/

#include "stdafx.h"
#define BOOST_SPIRIT_THREADSAFE

#include "boost/spirit.hpp"
#include "boost/spirit/phoenix/binders.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <comdef.h>
#include "DataDictionary.h"

using namespace std;
// Semantic actions can be functors. The operator() function is called
// with two iterators that specify the range of input that matches the production.

struct do_quit {
  template<typename Iter>
  void operator()(Iter, Iter) const
  {
    std::exit(EXIT_SUCCESS);
  }
};

// Symbol table for storing variables.
//typedef std::map<std::string, std::string> symtab_t;
typedef DataDictionary symtab_t;

struct calculator : boost::spirit::grammar<calculator>
{
  // The parser object is copied a lot, so instead of keeping its own table
  // of variables, it keeps track of a reference to a common table.
  calculator(symtab_t& variables) : variables(variables) {}

  // A production can have an associated closure, to store information
  // for that production.
  struct value_closure : boost::spirit::closure<value_closure, std::string>
  {
    member1 value;
  };

  struct assignment_closure :
    boost::spirit::closure<assignment_closure, std::string, std::string>
  {
    member1 name;
    member2 value;
  };

  struct string_closure : boost::spirit::closure<string_closure, std::string>
  {
    member1 name;
  };

  // Following is the grammar definition.
  template <typename ScannerT>
  struct definition
  {
    definition(calculator const& self)
    {
      using namespace boost::spirit;
      using namespace phoenix;

      // The commands are linked to functors or member functions,
      // to demonstrate both styles. In real code, you should choose
      // one style and use it uniformly.
      command
        | as_lower_d["quit"][do_quit()]
        ;

		STRING		=  ch_p('\"') >> lexeme_d[*( anychar_p - ch_p('\"') )] 
		[STRING.value =  construct_<std::string>(arg1, arg2)]
		>> ch_p('\"')
			; 

      // The lexeme_d directive tells the scanner to treat white space as
      // significant. Thus, an identifier cannot have internal white space.
      // The alpha_p and alnum_p parsers are built-in.
      // Notice how the semantic action uses a Phoenix lambda function
      // that constructs a std::string. The arg1 and arg2 placeholders are
      // are bound at runtime to the iterator range that matches this rule.
      identifier
        = lexeme_d
        [  
          ( alpha_p | '_')
          >> *( alnum_p | '_')
        ][identifier.name = construct_<std::string>(arg1, arg2)]
        ;

      group
        = '('
          >> expression[group.value = arg1]
          >> ')'
        ;

      // An assignment statement must store the variable name and value.
      // The name and the value are stored in the closure, then the define
      // function is called to store the definition. Notice how a rule can
      // have multiple semantic actions.
      assignment
        = identifier[assignment.name = arg1]
			 >> str_p(":=") // '='
			>> expression[assignment.value = arg1]
               [bind(&calculator::define)(self, assignment.name, assignment.value)]
        ;

      // A statement can end at the end of the line, or with a semicolon.
      statement
        =   ( command
            | assignment
            | expression[bind(&calculator::do_print)(self, arg1)]
          )
        >> (end_p | ';')
        ;

      // The longest_d directive is built-in to tell the parser to make
      // the longest match it can. Thus "1.23" matches real_p rather than
      // int_p followed by ".23".
      literal
        = longest_d
        [
          int_p[literal.value = bind(&calculator::convertint)(self, arg1)]
          | real_p[literal.value = bind(&calculator::convertdbl)(self, arg1)] // arg1)]
        ]
        ;

      // A variable name must be looked up. This is a straightforward
      // Phoenix binding.
      factor
        = STRING[factor.value=arg1]
		| literal[factor.value =  arg1]
        | group[factor.value =  arg1]
        | identifier[factor.value = bind(&calculator::lookup)(self, arg1)]
        ;

      term
        = (factor[term.value = arg1]
          >> *( ('*' >> factor[term.value = bind(&calculator::do_times)(self,  term.value, arg1)])
              | ('/' >> factor[term.value = bind(&calculator::do_div)(self,  term.value, arg1)])
            )
			)
			| ('!' >> factor[term.value = bind(&calculator::do_neg)(self,  arg1)])
			| (str_p("MAX") >> '(' >> factor[term.value = arg1] >> ',' >> factor[term.value = bind(&calculator::do_max)(self,  term.value, arg1)]) 
			| (str_p("MIN") >> '(' >> factor[term.value = arg1] >> ',' >> factor[term.value = bind(&calculator::do_min)(self,  term.value, arg1)]) 
        ;

      expression
        = term[expression.value = arg1]
          >> *( ('+' >> term[expression.value = bind(&calculator::do_plus)(self,  expression.value, arg1)])
              | ('-' >> term[expression.value = bind(&calculator::do_minus)(self,  expression.value, arg1)])
               | (str_p("==") >> term[expression.value = bind(&calculator::do_equals)(self,  expression.value, arg1)])
               | ("&&" >> term[expression.value = bind(&calculator::do_and)(self,  expression.value, arg1)])
               | ("||" >> term[expression.value = bind(&calculator::do_or)(self,  expression.value, arg1)])
           )
        ;
    }

    // The start symbol is returned from start().
    boost::spirit::rule<ScannerT> const&
    start() const { return statement; }

    // Each rule must be declared, optionally with an associated closure.
    boost::spirit::rule<ScannerT> command, statement;
    boost::spirit::rule<ScannerT, assignment_closure::context_t> assignment;
    boost::spirit::rule<ScannerT, string_closure::context_t> identifier;
    boost::spirit::rule<ScannerT, value_closure::context_t> expression, factor,
                                                            group, literal, term, STRING;
  };

  // Member functions that are called in semantic actions.
  void define(const std::string& name, std::string value) const
  {
    variables[name] = value;
  }

  std::string lookup(const std::string& name) const
  {
	  symtab_t::iterator it = variables.find(name);
	  if (it == variables.end()) 
	  {
		  std::stringstream str;
		  str << "undefined name: " << name << '\n';
		  OutputDebugString(str.str().c_str());
		  return "0.0";
	  }
	  else
		  return (*it).second;
  }

  void do_print(std::string x) const
  {
    OutputDebugString( x.c_str()); 
	OutputDebugString( "\n"); 
  }
  std::string do_math(std::string op, std::string v1, std::string v2) const
  {
	  if(v1.size() == 0)
		  v1="0";
	  if(v2.size() == 0)
		  v2="0";

	  double d1 = boost::lexical_cast<double>(v1);
	  double d2 = boost::lexical_cast<double>(v2);
	  double d3=0.0;
	  if(op=="plus")d3=d1+d2;
	  if(op=="minus")d3=d1-d2;
	  if(op=="times")d3=d1*d2;
	  if(op=="div" && d2 != 0.0 )d3=d1/d2;
	  if(op=="and") d3 = (((int) d1) && ((int)d2));
	  if(op=="or") d3 = (((int) d1) || ((int)d2));
	  if(op=="max") d3 = (d1>d2)?d1 : d2;
	  if(op=="min") d3 = (d1>d2)?d1 : d2;
	  if(op=="neg") d3 =  !d1;
	  return boost::lexical_cast<std::string>(d3);

  }
  std::string convertint(int int1) const { return boost::lexical_cast<std::string>(int1); }
  std::string convertdbl(double d1) const { return boost::lexical_cast<std::string>(d1); }
  std::string do_times(std::string v1, std::string v2) const { return do_math("times",v1,v2); }
  std::string do_div(std::string v1, std::string v2) const { return do_math("div",v1,v2); }
  std::string do_plus(std::string v1, std::string v2) const { return do_math("plus",v1,v2); }
  std::string do_minus(std::string v1, std::string v2) const { return do_math("minus",v1,v2); }
  std::string do_max(std::string v1, std::string v2) const { return do_math("max",v1,v2); }
  std::string do_min(std::string v1, std::string v2) const { return do_math("max",v1,v2); }
  std::string do_neg(std::string v1) const { return do_math("neg",v1,"0"); }



  std::string do_equals(std::string v1, std::string v2) const {
	  return boost::lexical_cast<std::string>(v1==v2);  
  }
  std::string do_and(std::string v1, std::string v2) const { return do_math("and",v1,v2); }
  std::string do_or(std::string v1, std::string v2) const { return do_math("or",v1,v2); }
  std::string convertquotedstring(std::string v1) const 
  { 
	  v1=v1.substr(1);
	  v1=v1.substr(0,v1.size()-1);
	  return v1; 
  }

  
  //void dump() const
  //{
  //  // Dump the entire symbol table. Notice how this function uses
  //  // Boost lambda functions instead of Phoenix, just to show you that
  //  // you can mix the two in a single file.
  //  using namespace boost::lambda;
  //  typedef std::pair<std::string, std::string> symtab_pair;
  //  for_each(variables.begin(), variables.end(),
  //    std::cout << bind(&symtab_pair::first, _1) << '=' <<
  //                 bind(&symtab_pair::second, _1) << '\n');
  //}

private:
  symtab_t& variables;
};

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
static boost::mutex io_mutex;

void Parse(std::string line, calculator & calc);
void Interpret(DataDictionary &vars, std::vector<std::string> lines)
{
	using namespace boost::spirit;
	using namespace std;
	symtab_t variables;
	variables["pi"] = "3.141592653589792";
	variables.insert(vars.begin(), vars.end());
	calculator calc(vars);
	boost::mutex::scoped_lock  lock(io_mutex);
	//  std::string line="a=2*3;";
	// lines.push_back("a:=2*3\n");
	// lines.push_back("c:=3+3\n");
	//  lines.push_back("f:=(2+2)*(1+1)\n");
	//lines.push_back("b:=\"IDLE\"\n");
	//lines.push_back("d:=(b==\"IDLE\")\n");
	//lines.push_back("d:=!d\n");
	//lines.push_back("g:=(c==6)\n");

	for(int i=0; i< lines.size(); i++)// (getline(cin, line))
	{
		Parse(lines[i], calc);
	}
	int n=0;
}

// Read one line of text and parse it. If the parser does not consume
// the entire string, keep parsing the same string until an error occurs
// or the string is consumed. Then go back and read another string.
void Parse(std::string line, calculator & calc)
{
	using namespace boost::spirit;
	using namespace std;
	string::iterator first = line.begin();
	parse_info<string::iterator> info;
	do {
		info = parse(first, line.end(), calc, space_p);
		if (! info.hit)
		{
			std::stringstream str;
			// Display a caret that points to the position where the error
			// was detected.
			str << setw(info.stop - line.begin()) << " " << "^ error\n";
			OutputDebugString(line.c_str());
			OutputDebugString(str.str().c_str());
		}
		else if (! info.full)
			// Keep track of where to start parsing the next statement.
			first = info.stop;
	} while(! info.full && info.hit);
}

