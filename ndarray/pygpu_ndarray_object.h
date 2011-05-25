/**
 * struct PyGPUArrayObject
 *
 * This is a Python type.  
 *
 */
#ifndef _PYGPU_NDARRAY_OBJECT_H
#define _PYGPU_NDARRAY_OBJECT_H

typedef struct PyGpuNdArrayObject{
  PyObject_HEAD

  GpuNdArray gpu_ndarray; //no pointer, just inlined.
  PyObject * base;
  PyArray_Descr * descr; // for numpy-like desc
  int data_allocated; //the number of bytes allocated for devdata
} PyGpuNdArrayObject;

#define PyGpuNdArray_NDIM(obj) (((PyGpuNdArrayObject *)obj)->gpu_ndarray.nd)
#define PyGpuNdArray_DATA(obj) (((PyGpuNdArrayObject *)obj)->gpu_ndarray.data)
#define PyGpuNdArray_BYTES(obj) (((PyGpuNdArrayObject *)obj)->gpu_ndarray.data)
#define PyGpuNdArray_OFFSET(obj) (((PyGpuNdArrayObject *)(obj))->gpu_ndarray.offset)
#define PyGpuNdArray_DIMS(obj) (((PyGpuNdArrayObject *)obj)->gpu_ndarray.dimensions)
#define PyGpuNdArray_STRIDES(obj) (((PyGpuNdArrayObject *)obj)->gpu_ndarray.strides)
#define PyGpuNdArray_DIM(obj,n) (PyGpuNdArray_DIMS(obj)[n])
#define PyGpuNdArray_STRIDE(obj,n) (PyGpuNdArray_STRIDES(obj)[n])
#define PyGpuNdArray_BASE(obj) (((PyGpuNdArrayObject *)obj)->base)
#define PyGpuNdArray_DESCR(obj) (((PyGpuNdArrayObject *)obj)->descr)
#define PyGpuNdArray_FLAGS(obj) (((PyGpuNdArrayObject *)obj)->gpu_ndarray.flags)
#define PyGpuNdArray_ITEMSIZE(obj) (((PyGpuNdArrayObject *)obj)->descr->elsize)
#define PyGpuNdArray_TYPE(obj) (((PyGpuNdArrayObject *)(obj))->descr->type_num)

#define PyGpuNdArray_SIZE(obj) PyArray_MultiplyList(PyGpuNdArray_DIMS(obj),PyGpuNdArray_NDIM(obj))
//npy_intp PyGpuNdArray_Size(PyObject* obj);
//npy_intp PyGpuNdArray_NBYTES(PyObject* arr);

/*
  Flags accessor
 */
#define PyGpuNdArray_CHKFLAGS(m, FLAGS)                              \
        ((((PyGpuNdArrayObject *)(m))->gpu_ndarray.flags & (FLAGS)) == (FLAGS))

#define PyGpuNdArray_ISCONTIGUOUS(m) PyGpuNdArray_CHKFLAGS(m, NPY_CONTIGUOUS)
#define PyGpuNdArray_ISFORTRAN(m) (PyGpuNdArray_CHKFLAGS(m, NPY_F_CONTIGUOUS) && \
                                   PyGpuNdArray_NDIM(m) > 1)
#define PyGpuNdArray_FORTRAN_IF(m) (PyGpuNdArray_CHKFLAGS(m, NPY_F_CONTIGUOUS)? \
                                    NPY_F_CONTIGUOUS : 0)
#define PyGpuNdArray_ISONESEGMENT(m) (PyGpuNdArray_NDIM(m) == 0 || \
                                      PyGpuNdArray_ISCONTIGUOUS(m) || \
                                      PyGpuNdArray_ISFORTRAN(m))
#define PyGpuNdArray_ISWRITEABLE(m) PyGpuNdArray_CHKFLAGS(m, NPY_WRITEABLE)
#define PyGpuNdArray_ISALIGNED(m) PyGpuNdArray_CHKFLAGS(m, NPY_ALIGNED)

#define PyGpuNdArray_ISNBO(arg) ((arg) != NPY_OPPBYTE)
// THE NEXT ONE SEEM BAD...
#define PyGpuNdArray_IsNativeByteOrder PyArray_ISNBO
#define PyGpuNdArray_ISNOTSWAPPED(m) PyArray_ISNBO(PyArray_DESCR(m)->byteorder)
#define PyGpuNdArray_FLAGSWAP(m, flags) (PyGpuNdArray_CHKFLAGS(m, flags) && PyGpuNdArray_ISNOTSWAPPED(m))

#define PyGpuNdArray_ISCARRAY(m) PyGpuNdArray_FLAGSWAP(m, NPY_CARRAY)
#define PyGpuNdArray_ISCARRAY_RO(m) PyGpuNdArray_FLAGSWAP(m, NPY_CARRAY_RO)
#define PyGpuNdArray_ISFARRAY(m) PyGpuNdArray_FLAGSWAP(m, NPY_FARRAY)
#define PyGpuNdArray_ISFARRAY_RO(m) PyGpuNdArray_FLAGSWAP(m, NPY_FARRAY_RO)
#define PyGpuNdArray_ISBEHAVED(m) PyGpuNdArray_FLAGSWAP(m, NPY_BEHAVED)
#define PyGpuNdArray_ISBEHAVED_RO(m) PyGpuNdArray_FLAGSWAP(m, NPY_ALIGNED)

static
void PyGpuNdArray_fprint(FILE * fd, const PyGpuNdArrayObject *self)
{
    fprintf(fd, "PyGpuNdArrayObject <%p, %p> nd=%i data_allocated=%d\n",
	    self, PyGpuNdArray_DATA(self), PyGpuNdArray_NDIM(self), self->data_allocated);
    fprintf(fd, "\tRefcount: %d\n", self->ob_refcnt);
    fprintf(fd, "\tBASE: %p\n", PyGpuNdArray_BASE(self));
    fprintf(fd, "\tHOST_DIMS:      ");
    for (int i = 0; i < PyGpuNdArray_NDIM(self); ++i)
    {
        fprintf(fd, "%i\t", PyGpuNdArray_DIMS(self)[i]);
    }
    fprintf(fd, "\n\tHOST_STRIDES: ");
    for (int i = 0; i < PyGpuNdArray_NDIM(self); ++i)
    {
        fprintf(fd, "%i\t", PyGpuNdArray_STRIDES(self)[i]);
    }
    fprintf(fd, "\n\tFLAGS: ");
    fprintf(fd, "\n\t\tC_CONTIGUOUS: %d", PyGpuNdArray_ISCONTIGUOUS(self));
    fprintf(fd, "\n\t\tPyGpuNdArray_ISFORTRAN: %d PyGpuNdArray_FORTRAN_IF:%d F_CONTIGUOUS: %d",
            PyGpuNdArray_ISFORTRAN(self), PyGpuNdArray_FORTRAN_IF(self), PyGpuNdArray_CHKFLAGS(self, NPY_FORTRAN));
    fprintf(fd, "\n\t\tF_CONTIGUOUS: %d", PyGpuNdArray_ISFORTRAN(self));
    fprintf(fd, "\n\t\tOWNDATA: %d", PyGpuNdArray_CHKFLAGS(self, NPY_OWNDATA));
    fprintf(fd, "\n\t\tWRITEABLE: %d", PyGpuNdArray_ISWRITEABLE(self));
    fprintf(fd, "\n\t\tALIGNED: %d", PyGpuNdArray_ISALIGNED(self));
    fprintf(fd, "\n\t\tUPDATEIFCOPY: %d", PyGpuNdArray_CHKFLAGS(self, NPY_UPDATEIFCOPY));
    fprintf(fd, "\n");

}
static
void PyArray_fprint(FILE * fd, const PyArrayObject *self)
{
    fprintf(fd, "PyArrayObject <%p, %p> nd=%i\n",
	    self, PyArray_DATA(self), PyArray_NDIM(self));
    fprintf(fd, "\tHOST_DIMS:      ");
    for (int i = 0; i < PyArray_NDIM(self); ++i)
    {
        fprintf(fd, "%i\t", PyArray_DIMS(self)[i]);
    }
    fprintf(fd, "\n\tHOST_STRIDES: ");
    for (int i = 0; i < PyArray_NDIM(self); ++i)
    {
        fprintf(fd, "%i\t", PyArray_STRIDES(self)[i]);
    }
    fprintf(fd, "\n\tFLAGS: ");
    fprintf(fd, "\n\t\tC_CONTIGUOUS: %d", PyArray_ISCONTIGUOUS(self));
    fprintf(fd, "\n\t\tPyArray_ISFORTRAN: %d PyArray_FORTRAN_IF:%d F_CONTIGUOUS: %d",
            PyArray_ISFORTRAN(self), PyArray_FORTRAN_IF(self), PyArray_CHKFLAGS(self, NPY_FORTRAN));
    fprintf(fd, "\n\t\tOWNDATA: %d", PyArray_CHKFLAGS(self, NPY_OWNDATA));
    fprintf(fd, "\n\t\tWRITEABLE: %d", PyArray_ISWRITEABLE(self));
    fprintf(fd, "\n\t\tALIGNED: %d", PyArray_ISALIGNED(self));
    fprintf(fd, "\n\t\tUPDATEIFCOPY: %d", PyArray_CHKFLAGS(self, NPY_UPDATEIFCOPY));
    fprintf(fd, "\n");

}

template <typename T>
static T ceil_intdiv(T a, T b)
{
    return (a/b) + ((a % b) ? 1: 0);
}
#endif
