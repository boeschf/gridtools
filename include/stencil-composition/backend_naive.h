#pragma once

#include <boost/mpl/has_key.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/find_if.hpp>
#include "../storage/storage.h"
#include "basic_token_execution.h"
#include "heap_allocated_temps.h"
#include "backend.h"

namespace gridtools {
    namespace _impl{
        template <>
        struct cout<_impl::Host>
        {
            template <typename T>
            const cout& operator << (T arg) const {std::cout<<arg; return *this;}
        };

    }//namespace _impl

    namespace _impl_host {

        template < typename Arguments >
        struct run_functor_host : public _impl::run_functor < run_functor_host< Arguments > >
        {

            typedef _impl::run_functor < run_functor_host < Arguments > > super;
            explicit run_functor_host(typename Arguments::domain_list_t& domain_list,  typename Arguments::coords_t const& coords)
                : super(domain_list, coords)
            {}

            explicit run_functor_host(typename Arguments::domain_list_t& domain_list,  typename Arguments::coords_t const& coords, int i, int j, int bi, int bj)
                : super(domain_list, coords, i, j, bi, bj)
            {}

        };
    }

    namespace _impl{

/** Partial specialization: naive and block implementation for the host backend */
        template <typename Arguments >
        struct execute_kernel_functor < _impl_host::run_functor_host< Arguments > >
        {
            typedef _impl_host::run_functor_host< Arguments > backend_t;
            template< typename Traits >
            static void execute_kernel( typename Traits::local_domain_t& local_domain, const backend_t * f )
                {
                    typedef typename Arguments::coords_t coords_t;
                    typedef typename Arguments::loop_intervals_t loop_intervals_t;
                    typedef typename Traits::range_t range_t;
                    typedef typename Traits::functor_t functor_t;
                    typedef typename Traits::local_domain_t  local_domain_t;
                    typedef typename Traits::interval_map_t interval_map_t;
                    typedef typename Traits::iterate_domain_t iterate_domain_t;


                for (int i = f->m_starti + range_t::iminus::value;
                     i < f->m_starti + f->m_BI + range_t::iplus::value;
                     ++i)
                    for (int j = f->m_startj + range_t::jminus::value;
                         j < f->m_startj + f->m_BJ + range_t::jplus::value;
                         ++j)
                    {
                        iterate_domain_t it_domain(local_domain, i,j, f->m_coords.template value_at<typename Traits::first_hit_t>());

                        gridtools::for_each<loop_intervals_t>
                            (_impl::run_f_on_interval
                             <functor_t,
                             interval_map_t,
                             iterate_domain_t,
                             coords_t>
                             (it_domain,f->m_coords)
                                );
                    }
                }

        };


//wasted code because of the lack of constexpr
        template <typename Arguments >
        struct backend_type< _impl_host::run_functor_host< Arguments > >
        {
            static const BACKEND m_backend=Host;
        };


        /**Traits struct, containing the types which are specific for the host backend*/
        template<>
        struct backend_from_id<Host>
        {
            template <typename ValueType, typename Layout>
            struct storage_traits{
                typedef storage<ValueType, Layout> storage_t;
            };

            template <typename Arguments>
            struct execute_traits
            {
                typedef _impl_host::run_functor_host< Arguments > backend_t;
            };

            //function alias (pre C++11, std::bind or std::mem_fn,
            //using function pointers looks very ugly)
            template<
                typename Sequence
                , typename F
                >
            //unnecessary copies/indirections if the compiler is not smart (std::forward)
            inline static void for_each(F f)
                {
                    gridtools::for_each<Sequence>(f);
                }

        };
    } //namespace _impl

} // namespace gridtools
