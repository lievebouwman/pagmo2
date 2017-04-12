/* Copyright 2017 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#ifndef PAGMO_RNG_HPP
#define PAGMO_RNG_HPP

#include <mutex>
#include <random>

namespace pagmo
{
namespace detail
{

// PaGMO makes use of the 32-bit Mersenne Twister by Matsumoto and Nishimura, 1998.
using random_engine_type = std::mt19937;

template <typename = void>
struct random_device_statics {
    /// PaGMO random engine
    static random_engine_type m_e;
    /// Mutex protecting access to PaGMO random engine
    static std::mutex m_mutex;
};

template <typename T>
random_engine_type random_device_statics<T>::m_e(static_cast<random_engine_type::result_type>(std::random_device()()));

template <typename T>
std::mutex random_device_statics<T>::m_mutex;

} // end namespace detail

/// Thread-safe random device
/**
 * This class intends to be a thread-safe substitute for std::random_device,
 * allowing, at the same time, precise global seed control throughout PaGMO.
 * It offers the user access to a global Pseudo Random Sequence generated by the
 * 32-bit Mersenne Twister by Matsumoto and Nishimura, 1998.
 * Such a PRS can be accessed by all PaGMO classes via the static method
 * random_device::next. The seed of this global Pseudo Random Sequence can
 * be set by the method random_device::set_seed, else by default is initialized
 * once at run-time using std::random_device.
 *
 * In PaGMO, all classes that contain a random engine (thus that generate
 * random numbers from variates), by default should contain something like:
 * @code{.unparsed}
 * #include "rng.hpp"
 * class class_using_random {
 * explicit class_using_random(args ...... , unsigned int seed = pagmo::random_device::next()) : m_e(seed),
 * m_seed(seed);
 * private:
 *    // Random engine
 *    mutable detail::random_engine_type               m_e;
 *    // Seed
 *    unsigned int                                     m_seed;
 * }
 * @endcode
 */
class random_device : public detail::random_device_statics<>
{
public:
    /// Next element of the Pseudo Random Sequence
    /**
     * This static method returns the next element of the PRS.
     *
     * @returns the next element of the PRS
     */
    static unsigned int next()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return static_cast<unsigned int>(m_e());
    }
    /// Sets the seed for the PRS
    /**
     * This static method sets a new seed for the PRS, so that all the
     * following calls to random_device::next() will always repeat the same
     * numbers.
     *
     * @param seed The new seed to be used
     */
    static void set_seed(unsigned int seed)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_e.seed(static_cast<detail::random_engine_type::result_type>(seed));
    }
};

} // end namespace pagmo

#endif