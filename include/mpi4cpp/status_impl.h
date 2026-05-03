// Copyright 2018 - 2026, Joonas Nättilä and the hel-astro-lab contributors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "status.h"

namespace mpi4cpp { namespace mpi {

inline bool 
status::cancelled() const
{
  int flag = 0;
  MPI_CHECK_RESULT(MPI_Test_cancelled, (&m_status, &flag));
  return flag != 0;
}



} } // ns mpi4cpp::mpi
