   body {counter-reset:section;}
    h1 {counter-reset:subsection;}
    h1:before
    {
    counter-increment:section;
    content:"Section " counter(section) ". ";
        font-weight:bold;
    }
    h2:before
    {
    counter-increment:subsection;
    content:counter(section) "." counter(subsection) " ";
    }