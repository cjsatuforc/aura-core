/**
 * \file: imu_vn100_spi.hxx
 *
 * VectorNav VN100_SPI (SPI) driver
 *
 * Copyright (C) 2012 - Curtis L. Olson - curtolson@flightgear.org
 *
 */

#pragma once

#include <pyprops.hxx>

#include <string>
using std::string;

#include "include/globaldefs.h"

void imu_vn100_spi_init( string output_path, pyPropertyNode *config );
bool imu_vn100_spi_get();
void imu_vn100_spi_close();
