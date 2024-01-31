/**
	@file
	plussz.c - one of the simplest max objects you can make -rdd 2001
	(plussz is/was the name of a Hungarian vitamin C tablet-drink from the early 90s)

	this example is provided for musicians who want to learn to write their own Max externals but who only
	have rudimentary computer programming skills and feel somewhat overwhelmed by the other examples in the Max SDK

	this object has 2 inlets and one outlet
	it responds to ints in its inlets and the 'bang' message in the left inlet
	it responds to the 'assistance' message sent by Max when the mouse is positioned over an inlet or outlet
		(including an assistance method is optional, but strongly sugggested)
	it adds its input values together and outputs their sum

	@ingroup	examples
*/

#include "ext.h"			// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"		// this is required for all objects using the newer style for writing objects.

typedef struct _avgsnapper {	// defines our object's internal variables for each instance in a patch
	t_object p_ob;			// object header - ALL objects MUST begin with this...
	long p_value0;			// int value - received from the left inlet and stored internally for each object instance
	long p_value1;			// int value - received from the middle inlet and stored internally for each object instance
    t_atom* p_list;            // list - received from the right inlet and stored internally for each object instance
	void *p_outlet;			// outlet creation - inlets are automatic, but objects must "own" their own outlets
} t_avgsnapper;


// these are prototypes for the methods that are defined below
void avgsnapper_bang(t_avgsnapper *x);
void avgsnapper_int(t_avgsnapper *x, long n);
void avgsnapper_in1(t_avgsnapper *x, long n);
void avgsnapper_list(t_avgsnapper *x, t_symbol *s, long argc, t_atom *argv);
void avgsnapper_assist(t_avgsnapper *x, void *b, long m, long a, char *s);
void *avgsnapper_new();


t_class *avgsnapper_class;		// global pointer to the object class - so max can reference the object


//--------------------------------------------------------------------------

void ext_main(void *r)
{
	t_class *c;

	c = class_new("avgsnapper", (method)avgsnapper_new, (method)NULL, sizeof(t_avgsnapper), 0L, 0);
	// class_new() loads our external's class into Max's memory so it can be used in a patch
	// avgsnapper_new = object creation method defined below

	class_addmethod(c, (method)avgsnapper_bang,		"bang",		0);			// the method it uses when it gets a bang in the left inlet
	class_addmethod(c, (method)avgsnapper_int,		"int",		A_LONG, 0);	// the method for an int in the left inlet (inlet 0)
	class_addmethod(c, (method)avgsnapper_in1,		"in1",		A_LONG, 0);	// the method for an int in the middle inlet (inlet 1)
    class_addmethod(c, (method)avgsnapper_list,		"list",		A_GIMME, 0);	// the method for a list in the right inlet (inlet 2)
	// "ft1" is the special message for floats
	class_addmethod(c, (method)avgsnapper_assist,	"assist",	A_CANT, 0);	// (optional) assistance method needs to be declared like this

	class_register(CLASS_BOX, c);
	avgsnapper_class = c;

	post("avgsnapper object loaded...",0);	// post any important info to the max window when our class is loaded
}


//--------------------------------------------------------------------------

void *avgsnapper_new()		// n = int argument typed into object box (A_DEFLONG) -- defaults to 0 if no args are typed
{
	t_avgsnapper *x;				// local variable (pointer to a t_avgsnapper data structure)

	x = (t_avgsnapper *)object_alloc(avgsnapper_class); // create a new instance of this object

	intin(x,1);					// create a second int inlet (leftmost inlet is automatic - all objects have one inlet by default)
    inlet_new(x, NULL);				// create a third list inlet -- replace this w a proxy
	x->p_outlet = intout(x);	// create an int outlet and assign it to our outlet variable in the instance's data structure

	x->p_value0	= 0;			// set initial (default) left operand value in the instance's data structure
	x->p_value1	= 0;			// set initial (default) middle operand value 

    // x->p_list = ;           // set initial (default) right operand value (n1 = variable passed to avgsnapper_new)

	post(" new avgsnapper object instance added to patch...",0); // post important info to the max window when new instance is created

	return(x);					// return a reference to the object instance
}


//--------------------------------------------------------------------------

void avgsnapper_assist(t_avgsnapper *x, void *b, long m, long a, char *s) // 4 final arguments are always the same for the assistance method
{
	if (m == ASSIST_OUTLET)
		sprintf(s,"Sum of All Inlets");
	else {
		switch (a) {
		case 0:
			sprintf(s,"Inlet %ld: Left Operand (Causes Output)", a);
			break;
		case 1:
			sprintf(s,"Inlet %ld: Middle Operand (Added to Left)", a);
			break;
        case 2:
            sprintf(s,"Inlet %ld: List to process", a);
            break;
        }
	}
}


void avgsnapper_bang(t_avgsnapper *x)	// x = reference to this instance of the object
{
	long sum;							// local variable for this method

	sum = x->p_value0+x->p_value1;		// add operands
	outlet_int(x->p_outlet, sum);		// send out the sum on bang
}


void avgsnapper_int(t_avgsnapper *x, long n)	// x = the instance of the object; n = the int received in the left inlet
{
	x->p_value0 = n;					        // store left operand value in instance's data structure
	avgsnapper_bang(x);						    // ... call the bang method to sum and send out our outlet
}


void avgsnapper_in1(t_avgsnapper *x, long n)	// x = the instance of the object, n = the int received in the middle inlet
{
	x->p_value1 = n;					        // just store middle operand value in instance's data structure and do nothing else
}

void avgsnapper_list(t_avgsnapper *x, t_symbol *s, long argc, t_atom *argv)	// x = the instance of the object, argc = number of atoms in the list, argv = the list
{
    long i;
    t_atom *ap;
 
    post("message selector is %s",s->s_name);
    post("there are %ld arguments",argc);
 
    // increment ap each time to get to the next atom
    for (i = 0, ap = argv; i < argc; i++, ap++) {
        switch (atom_gettype(ap)) {
            case A_LONG:
                post("%ld: %ld",i+1,atom_getlong(ap));
                break;
            case A_FLOAT:
                post("%ld: %.2f",i+1,atom_getfloat(ap));
                break;
            case A_SYM:
                post("%ld: %s",i+1, atom_getsym(ap)->s_name);
                break;
            default:
                post("%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
                break;
        }
    }					        
}
