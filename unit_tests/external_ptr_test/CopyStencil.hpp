/**
 * This code was automatically generated by gridtools4py:
 * the Python interface to the Gridtools library
 *
 */
#pragma once

#include <stencil-composition/stencil-composition.hpp>

#ifdef BACKEND_BLOCK
#define BACKEND backend<Host, GRIDBACKEND, Block >
#else
#define BACKEND backend<Host, GRIDBACKEND, Naive >
#endif

using gridtools::level;
using gridtools::accessor;
using gridtools::extent;
using gridtools::arg;

using namespace gridtools;
using namespace enumtype;


namespace copystencil_python
{
//
// definition of the special regions in the vertical (k) direction
//
typedef gridtools::interval<level<0,-1>, level<1,-1> > x_interval;
typedef gridtools::interval<level<0,-2>, level<1,1> > axis;

//
// the definition of the operators that compose a multistage stencil
// is extracted from the AST analysis of the loop comprehensions
// in Python, which use the 'kernel' function as a starting point
//
struct functor_4647
{
    //
    // the number of arguments of this functor
    //
    static const int n_args = 2;

    //
    // the input data fields of this functor are marked as 'const'
    //
    typedef accessor<0> in_data;
    typedef accessor<1, enumtype::inout> out_data;
    //
    // the ordered list of arguments of this functor
    //
    typedef boost::mpl::vector<in_data,out_data> arg_list;

    //
    // the operation of this functor
    //
    template <typename Evaluation>
    GT_FUNCTION
    static void Do(Evaluation const & eval, x_interval)
    {
        eval(out_data( )) = eval(in_data( ));
    }
};


//
// the following operator is provided for debugging purposes
//
std::ostream& operator<<(std::ostream& s, functor_4647 const)
{
    return s << "functor_4647";
}


bool test (uint_t d1, uint_t d2, uint_t d3,void *in_data_buff,void *out_data_buff)
{
    //
    // C-like memory layout
    //
    typedef gridtools::layout_map<0,1,2> layout_t;
    typedef gridtools::BACKEND::storage_info<0, layout_t> meta_t;

    //
    // define the storage unit used by the backend
    //
    typedef gridtools::BACKEND::storage_type<float_type, meta_t >::type storage_type;

    meta_t meta_((uint_t) 3,(uint_t) 2,(uint_t) 1);
    //
    // parameter data fields use the memory buffers received from NumPy arrays
    //
    storage_type in_data ( meta_, (float_type *) in_data_buff,
                              "in_data");
    storage_type out_data ( meta_,
                                 (float_type *) out_data_buff,
                                 "out_data");

    //
    // place-holder definition: their order matches the stencil parameters,
    // especially the non-temporary ones, during the construction of the domain
    //
    typedef arg<0,storage_type> p_in_data;
    typedef arg<1,storage_type> p_out_data;
    //
    // an array of placeholders to be passed to the domain
    //
    typedef boost::mpl::vector<p_in_data, p_out_data> accessor_list;

    //
    // construction of the domain.
    // The domain is the physical domain of the problem, with all the physical
    // fields that are used, temporary and not.
    // It must be noted that the only fields to be passed to the constructor
    // are the non-temporary. The order in which they have to be passed is the
    // order in which they appear scanning the placeholders in order.
    // (I don't particularly like this)
    //
    gridtools::domain_type<accessor_list> domain (boost::fusion::make_vector (&in_data, &out_data));

    //
    // definition of the physical dimensions of the problem.
    // The constructor takes the horizontal plane dimensions,
    // while the vertical ones are set according the the axis
    // property soon after this:
    //
    //      gridtools::grid<axis> grid(2,d1-2,2,d2-2);
    //
    uint_t di[5] = {0, 0, 0, d1-1, d1};
    uint_t dj[5] = {0, 0, 0, d2-1, d2};

    gridtools::grid<axis> grid(di, dj);
    grid.value_list[0] = 0;
    grid.value_list[1] = d3-1;

    //
    // Here we do a lot of stuff
    //
    // 1) we pass to the intermediate representation ::run function the
    // description of the stencil, which is a multi-stage stencil (mss);
    // 2) the logical physical domain with the fields to use;
    // 3) the actual domain dimensions
    //
    boost::shared_ptr<gridtools::computation> comp_copystencil =
      gridtools::make_computation<gridtools::BACKEND>
        (
            gridtools::make_mss
            (
                execute<forward>(),
                gridtools::make_esf<functor_4647>(p_in_data(), p_out_data())
                ),
            domain, grid
            );

    //
    // execute the stencil
    //
    comp_copystencil->ready();
    comp_copystencil->steady();
    comp_copystencil->run();

    //
    // clean everything up
    //
    comp_copystencil->finalize();

    return EXIT_SUCCESS;
}

} // namespace copystencil_python

extern "C"
{
    int run (uint_t dim1, uint_t dim2, uint_t dim3,
            void *in_data_buff,void *out_data_buff)
    {
        return !copystencil_python::test (dim1, dim2, dim3,
                                                in_data_buff,out_data_buff);
    }
}


    /**
 * Entry point of the test case
 */
bool test_copystencil_python ( )
{
    int d1 = 3;
    int d2 = 2;
    int d3 = 1;

    double *in_dat  = (double *) malloc (d1*d2*d3*sizeof (double));
    double *out_dat = (double *) malloc (d1*d2*d3*sizeof (double));

    for (int i=0; i<d1*d2*d3; i++)
        in_dat[i] = i + 1.0;

    run (d1, d2, d3, in_dat, out_dat);

    for (int i=0; i<d1*d2*d3; i++)
    {
        assert (in_dat[i] != 0.0);
#ifdef DOUBLE_PRECISION //hack while waititng for a proper handling of arbitrary precision floats from the python interface
        assert (in_dat[i] == out_dat[i]);
#endif
    }

#ifdef VERBOSE
    std::cout << "Copied " << d1*d2*d3 << " values ... ok!" << std::endl;
#endif

    return EXIT_SUCCESS;
}
