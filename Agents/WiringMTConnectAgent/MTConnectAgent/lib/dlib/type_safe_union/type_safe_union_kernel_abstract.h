// Copyright (C) 2009  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_TYPE_SAFE_UNION_KERNEl_ABSTRACT_
#ifdef DLIB_TYPE_SAFE_UNION_KERNEl_ABSTRACT_

#include "../algs.h"
#include "../noncopyable.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <
        typename T1,
        typename T2 = T1,
        typename T3 = T1,
        typename T4 = T1,
        typename T5 = T1, 
        typename T6 = T1,
        typename T7 = T1,
        typename T8 = T1,
        typename T9 = T1,
        typename T10 = T1,
        typename T11 = T1,
        typename T12 = T1,
        typename T13 = T1,
        typename T14 = T1,
        typename T15 = T1,
        typename T16 = T1,
        typename T17 = T1,
        typename T18 = T1,
        typename T19 = T1,
        typename T20 = T1
        >
    class type_safe_union : noncopyable
    {
        /*!
            REQUIREMENTS ON ALL TEMPLATE ARGUMENTS
                All template arguments must be default constructable and have
                a global swap.

            INITIAL VALUE
                - is_empty() == true
                - contains<U>() == false, for all possible values of U

            WHAT THIS OBJECT REPRESENTS 
                This object is a type safe analogue of the classic C union object. 
                The type_safe_union, unlike a union, can contain non-POD types such 
                as std::string.  

                For example:
                    union my_union
                    {
                        int a;
                        std::string b;   // Error, std::string isn't a POD
                    };

                    type_safe_union<int,std::string> my_type_safe_union;  // No error
        !*/

    public:

        type_safe_union(
        );
        /*!
            ensures
                - this object is properly initialized
        !*/

        ~type_safe_union(
        );
        /*!
            ensures
                - all resources associated with this object have been freed
        !*/

        template <typename T>
        static int get_type_id (
        );
        /*!
           ensures
              - if (T is the same type as one of the template arguments) then
                 - returns a number indicating which template argument it is.
                   (e.g. if T is the same type as T3 then this function returns 3)
               - else
                  - returns -1
        !*/

        template <typename T>
        bool contains (
        ) const;
        /*!
            ensures
                - if (this type_safe_union currently contains an object of type T) then
                    - returns true
                - else
                    - returns false
        !*/

        bool is_empty (
        ) const;
        /*!
            ensures
                - if (this type_safe_union currently contains any object at all) then
                    - returns true
                - else
                    - returns false
        !*/

        template <typename T>
        void apply_to_contents (
            T& obj
        );
        /*!
            requires
                - obj is a function object capable of operating on all the types contained
                  in this type_safe_union.  I.e.  obj(this->get<U>()) must be a valid
                  expression for all the possible U types.
            ensures
                - if (is_empty() == false) then
                    - Let U denote the type of object currently contained in this type_safe_union
                    - calls obj(this->get<U>())
        !*/

        template <typename T>
        void apply_to_contents (
            const T& obj
        );
        /*!
            requires
                - obj is a function object capable of operating on all the types contained
                  in this type_safe_union.  I.e.  obj(this->get<U>()) must be a valid
                  expression for all the possible U types.
            ensures
                - if (is_empty() == false) then
                    - Let U denote the type of object currently contained in this type_safe_union
                    - calls obj(this->get<U>())
        !*/

        template <typename T> 
        T& get(
        );
        /*!
            requires
                - T must be one of the types given to this object's template arguments
            ensures
                - #is_empty() == false
                - #contains<T>() == true
                - if (contains<T>() == true)
                    - returns a non-const reference to the object contained in this type_safe_union.
                - else
                    - Constructs an object of type T inside *this
                    - Any previous object stored in this type_safe_union is destructed and its
                      state is lost.
                    - returns a non-const reference to the newly created T object.

        !*/

        void swap (
            type_safe_union& item
        );
        /*!
            ensures
                - swaps *this and item
        !*/

    };

// ----------------------------------------------------------------------------------------

    template < ...  >
    inline void swap (
        type_safe_union<...>& a, 
        type_safe_union<...>& b 
    ) { a.swap(b); }   
    /*!
        provides a global swap function
    !*/

// ----------------------------------------------------------------------------------------

    template < ... >
    void serialize (
        const type_safe_union<...>& item, 
        std::ostream& out 
    );   
    /*!
        provides serialization support 

        Note that type_safe_union objects are serialized as follows:
         - if (item.is_empty()) then
            - perform: serialize(0, out)
         - else
            - perform: serialize(item.get_type_id<type_of_object_in_item>(), out);
                       serialize(item.get<type_of_object_in_item>(), out);
    !*/

    template < ...  >
    void deserialize (
        type_safe_union<...>& item, 
        std::istream& in
    );   
    /*!
        provides deserialization support 
    !*/

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_TYPE_SAFE_UNION_KERNEl_ABSTRACT_

