/* ---------------------------------------------------------------------------

  carray/rmagick/carray_rmagick.c

  This file is part of Ruby/CArray extension library.
  You can redistribute it and/or modify it under the terms of
  the Ruby Licence.

  Copyright (C) 2005-2008  Hiroki Motoyoshi

---------------------------------------------------------------------------- */

#include "magick/api.h"

#undef PACKAGE_VERSION
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_BUGREPORT
#undef PACKAGE_TARNAME

#include "carray.h"

static VALUE
Image_fetch_index (VALUE self, VALUE vidx)
{
  volatile VALUE out;
  Image *image;
  PixelPacket *pixel;
  int32_t irow, icol, imem;

  Data_Get_Struct(self, Image, image);

  Check_Type(vidx, T_ARRAY);
  irow    = NUM2INT(rb_ary_entry(vidx, 0));
  icol    = NUM2INT(rb_ary_entry(vidx, 1));
  imem    = NUM2INT(rb_ary_entry(vidx, 2));

  pixel = (PixelPacket *) GetVirtualPixels(image, 
                                     icol, irow, 1, 1, &image->exception);

  switch ( imem ) {
  case 0: out = ULONG2NUM(pixel->red); break;
  case 1: out = ULONG2NUM(pixel->green); break;
  case 2: out = ULONG2NUM(pixel->blue); break;
  case 3: out = ULONG2NUM(pixel->opacity); break;
  }

  return out;
}

static VALUE
Image_store_index (VALUE self, VALUE vidx, VALUE vval)
{
  Image *image;
  PixelPacket *pixel;
  int32_t irow, icol, imem;

  Data_Get_Struct(self, Image, image);

  Check_Type(vidx, T_ARRAY);
  irow    = NUM2INT(rb_ary_entry(vidx, 0));
  icol    = NUM2INT(rb_ary_entry(vidx, 1));
  imem    = NUM2INT(rb_ary_entry(vidx, 2));

  pixel = (PixelPacket *) GetVirtualPixels(image, 
                                     icol, irow, 1, 1, &image->exception);

  switch ( imem ) {
  case 0: pixel->red     = NUM2ULONG(vval); break;
  case 1: pixel->green   = NUM2ULONG(vval); break;
  case 2: pixel->blue    = NUM2ULONG(vval); break;
  case 3: pixel->opacity = NUM2ULONG(vval); break;
  }

  SyncAuthenticPixels(image, &image->exception);  

  return vval;
}


static VALUE
Image_copy_data_to_ca (VALUE self, VALUE vca)
{
  Image *image;
  PixelPacket *pixels;
  long columns, rows;
  long size, n;
  CArray  *ca;
  int32_t  dim[3];

  Data_Get_Struct(self, Image, image);
  columns = image->columns;
  rows    = image->rows;
  size    = columns * rows;
  dim[0] = rows;
  dim[1] = columns;
  dim[2] = 4;

  rb_check_carray_object(vca);
  Data_Get_Struct(vca, CArray, ca);
  ca_check_shape(ca, 3, dim);

  switch ( sizeof(Quantum) ) {
  case 1: 
    ca_check_data_type(ca, CA_UINT8);  
    break;
  case 2: 
    ca_check_data_type(ca, CA_UINT16);  
    break;
  case 4: 
    ca_check_data_type(ca, CA_UINT32);  
    break;
  default:
    rb_raise(rb_eRuntimeError, "invalid data_type of given carray");
  }

  SetImageType(image, TrueColorType); 

  pixels = (PixelPacket *) GetVirtualPixels(image, 
                                            0, 0, columns, rows, &image->exception);

  if ( ! pixels ) {
    rb_raise(rb_eRuntimeError, "failed to get image pixels");
  }

  ca_attach(ca);

  switch ( sizeof(Quantum) ) {
  case 1: {
    PixelPacket *p;
    u_int8_t *q;
    p = pixels;
    q = (u_int8_t *)ca->ptr;
    for (n=0; n<size; n++) {
      *q++ = p->red;
      *q++ = p->green;
      *q++ = p->blue;
      *q++ = p->opacity;
      p++;
    }
    break;
  }
  case 2: {
    PixelPacket *p;
    u_int16_t *q;
    p = pixels;
    q = (u_int16_t *)ca->ptr;
    for (n=0; n<size; n++) {
      *q++ = p->red;
      *q++ = p->green;
      *q++ = p->blue;
      *q++ = p->opacity;
      p++;
    }
    break;
  }
  case 4: {
    PixelPacket *p;
    u_int32_t *q;
    p = pixels;
    q = (u_int32_t *)ca->ptr;
    for (n=0; n<size; n++) {
      *q++ = p->red;
      *q++ = p->green;
      *q++ = p->blue;
      *q++ = p->opacity;
      p++;
    }
    break;
  }
  }

  ca_sync(ca);
  ca_detach(ca);

  return vca;
}

static VALUE
Image_sync_data_from_ca (VALUE self, VALUE vca)
{
  Image *image;
  PixelPacket *pixels;
  long columns, rows;
  long size, n;
  int okay;
  CArray  *ca;
  int32_t  dim[3];

  Data_Get_Struct(self, Image, image);
  columns = image->columns;
  rows    = image->rows;
  size    = columns * rows;
  dim[0]  = rows;
  dim[1]  = columns;
  dim[2]  = 4;

  rb_check_carray_object(vca);
  Data_Get_Struct(vca, CArray, ca);
  ca_check_shape(ca, 3, dim);

  switch ( sizeof(Quantum) ) {
  case 1: 
    ca_check_data_type(ca, CA_UINT8);  
    break;
  case 2: 
    ca_check_data_type(ca, CA_UINT16);  
    break;
  case 4: 
    ca_check_data_type(ca, CA_UINT32);  
    break;
  default:
    rb_raise(rb_eRuntimeError, "invalid data_type of given carray");
  }

  SetImageType(image, TrueColorType); 

  pixels = (PixelPacket *) GetVirtualPixels(image, 
                                              0, 0, columns, rows, &image->exception);

  if ( pixels ) {
    PixelPacket *p = pixels;
    ca_attach(ca);
    switch ( sizeof(Quantum) ) {
    case 1: {
      u_int8_t *q = (u_int8_t *) ca->ptr;
      for (n=0; n<size; n++) {
        p->red     = *q++;
        p->green   = *q++;
        p->blue    = *q++;
        p->opacity = *q++;
        p++;
      }
      break;
    }
    case 2: {
      u_int16_t *q = (u_int16_t *) ca->ptr;
      for (n=0; n<size; n++) {
        p->red     = *q++;
        p->green   = *q++;
        p->blue    = *q++;
        p->opacity = *q++;
        p++;
      }
      break;
    }
    case 4: {
      u_int32_t *q = (u_int32_t *) ca->ptr;
      for (n=0; n<size; n++) {
        p->red     = *q++;
        p->green   = *q++;
        p->blue    = *q++;
        p->opacity = *q++;
        p++;
      }
      break;
    }
    }

    ca_detach(ca);    

    okay = SyncAuthenticPixels(image, &image->exception);  

    if ( ! okay ) {
      rb_raise(rb_eRuntimeError, "image pixels could not be synced");
    }
    
  }

  return self;
}


static VALUE
Image_fill_data (VALUE self, VALUE vval)
{
  Image *image;
  PixelPacket *pixels;
  unsigned long val;
  long columns, rows;
  long size, n;
  int okay;

  Data_Get_Struct(self, Image, image);
  columns = image->columns;
  rows    = image->rows;
  size    = columns * rows;

  val     = NUM2ULONG(vval);

  SetImageType(image, TrueColorType); 

  pixels = (PixelPacket *) GetAuthenticPixels(image, 0, 0, columns, rows, &image->exception);

  if ( pixels ) {
    PixelPacket *p = pixels;
    switch ( sizeof(Quantum) ) {
    case 1: {
      for (n=0; n<size; n++) {
        p->red     = val;
        p->green   = val;
        p->blue    = val;
        p->opacity = val;
        p++;
      }
      break;
    }
    case 2: {
      for (n=0; n<size; n++) {
        p->red     = val;
        p->green   = val;
        p->blue    = val;
        p->opacity = val;
        p++;
      }
      break;
    }
    case 4: {
      for (n=0; n<size; n++) {
        p->red     = val;
        p->green   = val;
        p->blue    = val;
        p->opacity = val;
        p++;
      }
      break;
    }
    }

    okay = SyncAuthenticPixels(image, &image->exception);  

    if ( ! okay ) {
      rb_raise(rb_eRuntimeError, "image pixels could not be synced");
    }
    
  }

  return self;
}

void
Init_carray_rmagick ()
{
  VALUE mMagick = rb_const_get(rb_cObject, rb_intern("Magick")); 
  VALUE cImage  = rb_const_get(mMagick, rb_intern("Image"));

  rb_define_method(cImage, "_fetch_index",   Image_fetch_index, 1);
  rb_define_method(cImage, "_store_index",   Image_store_index, 2);
  rb_define_method(cImage, "_copy_data_to_ca",   Image_copy_data_to_ca, 1);
  rb_define_method(cImage, "_sync_data_from_ca", Image_sync_data_from_ca, 1);
  rb_define_method(cImage, "_fill_data", Image_fill_data, 1);
}

