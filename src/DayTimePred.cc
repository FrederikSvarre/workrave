// DayTimePred.cc --- Daily Time Predicate
//
// Copyright (C) 2001, 2002, 2003 Rob Caelers <robc@krandor.org>
// All rights reserved.
//
// Time-stamp: <2003-01-05 16:05:29 robc>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

static const char rcsid[] = "$Id$";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <stdio.h>

#include "DayTimePred.hh"

//! Sets the last time the predicate matched.
void
DayTimePred::set_last(time_t lastTime)
{
  last_time = lastTime;
  
  time_t now = time(NULL);

  if (last_time == 0)
    {
      last_time = now;
    }
}


int
DayTimePred::time_cmp(int h1, int m1, int h2, int m2)
{
  if (h1 < h2)
    return -1;
  else if (h1 > h2)
    return 1;

  if (m1 < m2)
    return -1;
  else if (m1 > m2)
    return 1;

  return 0;
}


bool
DayTimePred::init(int hour, int min)
{
  pred_hour = hour;
  pred_min = min;
  
  return true;
}


bool
DayTimePred::init(std::string spec)
{
  bool ret = false;
  std:: string::size_type pos = spec.find(':');

  if (pos != std::string::npos)
    {
      std::string hours;
      std::string minutes;
  
      hours = spec.substr(0, pos);
      minutes = spec.substr(pos + 1);

      ret = init(atoi(hours.c_str()), atoi(minutes.c_str()));
    }
  
  return ret;
}


int
DayTimePred::days_in_month(int month, int year)
{
  int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if (month == 1)
    {
      // Feb

      if (year % 4 == 0 && ( (year % 100 != 0) || (year % 400) == 0))
        {
          return 29;
        }
      else
        {
          return 28;
        }
    }
  else
    {
      return days[month];
    }
}


time_t
DayTimePred::get_time_offset()
{
  return pred_hour*60*60 + pred_min*60;
}


time_t
DayTimePred::get_next()
{
  struct tm *ret;

  ret = localtime(&last_time);
  
  if (ret != NULL)
    {
      if (time_cmp(ret->tm_hour, ret->tm_min, pred_hour, pred_min) >= 0)
        {
          ret->tm_mday++;
        }

      ret->tm_sec = 0;
      ret->tm_min = pred_min;
      ret->tm_hour = pred_hour;

      if (ret->tm_mday > days_in_month(ret->tm_mon, ret->tm_year + 1900))
        {
          ret->tm_mday = 1;
          ret->tm_mon++;
        }

      if (ret->tm_mon > 11)
        {
          ret->tm_mon = 0;
          ret->tm_year++;
        }

      return mktime(ret);
    }
  else
    {
      return 0;
    }
}


string
DayTimePred::to_string() const
{
  char buf[16];
  sprintf(buf, "day/%d:%02d", pred_hour, pred_min);
  return string(buf);
}
