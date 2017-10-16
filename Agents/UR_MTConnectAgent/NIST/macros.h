#ifndef MACROS_H
#define MACROS_H

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include <stdarg.h>

#ifdef C11
#include <bind>
#include <functional>
#include <mutex>
#include <shared_ptr>
#include <thread>

#else
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
typedef boost::mutex MUTEX;
#endif

// Microsoft specific push/pop macros
#pragma push_macro("SCOPED_LOCK")
#undef SCOPED_LOCK
//#define SCOPED_LOCK   boost::mutex::scoped_lock lock(*(the_mutex.get()));
#define SCOPED_LOCK

enum MESSAGE_MAP_TYPE { MESSAGE_MAP_THIS = 1, MESSAGE_MAP_BASE = 2 };

struct PropertyMap {
	MESSAGE_MAP_TYPE type;
	char *mName;
	size_t mOffset;
	int mSize;
	bool bIsArray;
	int mElementSize;
	char *mFormat;
	// Used for reference to base class...
#ifdef C11
	std::function<PropertyMap *()> mfPtrPropMap;
#else
	boost::function<PropertyMap *()> mfPtrPropMap;
#endif
	int *mArraySize;
};

#define Q(x) #x

// https://stackoverflow.com/questions/13180842/how-to-calculate-offset-of-a-class-member-at-compile-time
//  offsetOf(&X::c);
template <typename T, typename U> size_t offsetOf(U T::*member) {
	return (char *)&((T *)nullptr->*member) - (char *)nullptr;
}

#define BEGIN_SERIAL_PROP_MAP(X)                                               \
	static const char *msgname() { return Q(X); }                                \
	PropertyMap *GetPropertyMap() \
{                                           \
	typedef X MyClass;                                                         \
	static PropertyMap pPropMap[] = \
{

#define PROP_SERIAL_ENTRY(mName, pformat)                                      \
	\
{MESSAGE_MAP_THIS, Q(mName), offsetOf(&MyClass::mName),                        \
	sizeof(mName),    0,        0,                                           \
	pformat,          0,        0},

// Divde sizeof by size to get size of item
#define PROP_SERIAL_ENTRY_ARRAY(mName, mSize, pformat)                         \
	\
{MESSAGE_MAP_THIS, Q(mName), offsetOf(&MyClass::mName),                        \
	sizeof(mName),    1,        mSize,                                       \
	pformat,          0,        0},

#define PROP_SERIAL_ENTRY_VARARRAY(mName, mSize, pformat, var)                 \
	\
{MESSAGE_MAP_THIS, Q(mName), offsetOf(&MyClass::mName),                        \
	sizeof(mName),    1,        mSize,                                       \
	pformat,          0,        var},

#define PROP_SERIAL_ENTRY_BASE(mName)                                          \
	\
{MESSAGE_MAP_BASE,                                                             \
	Q(mName),                                                                \
	0,                                                                       \
	0,                                                                       \
	0,                                                                       \
	0,                                                                       \
	0,                                                                       \
	boost::bind(&mName::GetPropertyMap, this),                               \
	0},

#define END_SERIAL_PROP_MAP()                                                  \
	\
{                                                                         \
	MESSAGE_MAP_THIS, NULL, 0, 0, 0, 0, "%d", 0, 0                             \
}                                                                            \
	\
}                                                                         \
	;                                                                            \
	return pPropMap;                                                             \
	\
}

/**
\brief printf like string formatting

\param[in] *fmt : format of printf
\param[in]  varargs
\result     formatted string
*/
inline std::string StrFormat(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	int m;
	int n = (int)strlen(fmt) + 1028;
	std::string tmp(n, '0');
	while ((m = vsnprintf(&tmp[0], n - 1, fmt, argptr)) < 0) {
		n = n + 1028;
		tmp.resize(n, '0');
	}
	va_end(argptr);
	return tmp.substr(0, m);
}

inline bool is_bigendian() {
	static const int bsti = 1; // Byte swap test integer
	return ((*(char *)&bsti) == 0);
}
/**
\brief In-place swapping of bytes

\param[in/out] *object : memory to swap in-place
\param[in]     _size   : length in bytes
*/
inline void *swapbytes(void *_object, size_t _size) {
	long abcd = 0xabcd;
	// network order is big endian
	if (is_bigendian())
		return _object; // no swap necessary?
	unsigned char *start, *end;

	if (_size == 1)
		return _object;

	for (start = (unsigned char *)_object, end = start + _size - 1; start < end;
		++start, --end) {
			unsigned char swap = *start;
			*start = *end;
			*end = swap;
	}
	return _object;
}

/**
* @brief bscopy byte copy from to given size. Will swap bytes from network byte
* order
* into local byte order
* @param to  char address of to
* @param from char address of from
* @param mSize in chars of bytes to copy
* @return size
*/

inline size_t bscopy(char *to, char *from, size_t size) {
	memcpy(to, from, size);
	swapbytes(to, size);
	return size;
}
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
/**
* @brief The struct_decoder struct is useful for reading/writing network byte
* streams.
* It uses the preprocessor definition to define a property table describing
* each
* entry so they can easily be read/write/print including byte swapping if
* required.
* Uses the Curiously recurring template pattern to handle accessing the
* outermost property
* table.
*/
template <class T> struct struct_decoder 
{
//	boost::shared_mutex  mMutex; 	//!<  pointer to mutex to make read/write bytes thread safe
	bool bHasData; //!< flag to indicate that byte stream has been read
	/**
	* @brief struct_decoder<T> empty constructor that creates mutex and set has
	* data to false.
	* Does not intialize any data.
	*/
	struct_decoder<T>()
	{
		bHasData = false;
	}
	/**
	* @brief msglength scans through property table to dynamically compute
	* message size.
	* @return  size
	*/
	size_t	msglength()
	{
//			boost::shared_lock< boost::shared_mutex > lock(mMutex);
			int length = 0;
			PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();
			for (size_t i = 0; props[i].mName != NULL; i++) {
				if (props[i].bIsArray && props[i].mArraySize != NULL)
					length = (*props[i].mArraySize) * props[i].mElementSize;
				else
					length += props[i].mSize;
			}
			return length;
	}

	/**
	* @brief read transfers the buffer contents into the items as defined by
	* the property table for the contained structure.
	* @param inbuf  a signed or unsigned character array of bytes
	*/
	template <typename U> void read(U inbuf) 
	{
//		boost::shared_lock< boost::shared_mutex > lock(mMutex);
		bHasData = true;
		// pointer to this is used to add to variable/array offset
		char *pThis = (char *)this; // assume consecutive ordering classes

		T *debugThis = static_cast<T *>(this);

		// property map for this class - should get outer most property map
		PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();

		// Pointer to input memory buffer
		char *ptr = (char *)inbuf; // dynamic_cast<char *>(inbuf);

		// Cycle through each property in property map, and save value
		// Swap bytes if not in local PC byte order
		for (size_t i = 0; props[i].mName != NULL; i++) {
			// If not array, read a variable
			if (!props[i].bIsArray) {
				PropertyMap p = props[i];
				memcpy(pThis + props[i].mOffset, ptr, props[i].mSize);
				swapbytes(pThis + props[i].mOffset, props[i].mSize);
				ptr += props[i].mSize;
			} else {
				bool bVector = false;
				// Write an array of simple values
				// m contains array size
				size_t m = props[i].mSize;

				// if dynamic array size - lookup var pointer value
				if (props[i].mArraySize != NULL) {
					// This is actual size of array, not max amount
					// Should check less that allocated array
					m = (*props[i].mArraySize) * props[i].mElementSize;
					bVector = true;
				}

				// Get start of array offset
				char *aptr = pThis + props[i].mOffset;

				// Size of each element in array
				size_t n = props[i].mElementSize;

				char *arrayptr = ptr;
				for (size_t j = 0; j < m; j = j + n) {
					// vector only allocates 8 bytes as pointer to heap pointer?
					memcpy(aptr, arrayptr, n);

					// fixme: this won't work for struct with e.g., multiple doubles.
					swapbytes(aptr, n);

					aptr += n;
					arrayptr += n;
				}
				// Now we skip to the next array slot
				ptr += props[i].mSize;
			}
		}
	}
	/**
	* @brief write transfers contents of the items as defined by property table
	* into the out
	* buffer.
	* @param outbuf is  a signed or unsigned character array of bytes that
	* assumes
	* enough length is allocated to copy in entire struct contents.
	*/
	void write(char *outbuf) 
	{
//		boost::unique_lock< boost::shared_mutex > lock(mMutex);

		// FIXME: add lenth of outbuf and assert length>msglength

		char *pThis = (char *)this; // assume consecutive ordering classes
		PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();
		char *ptr = outbuf;
		for (size_t i = 0; props[i].mName != NULL; i++) {
			if (!props[i].bIsArray) {
				memcpy(ptr, pThis + props[i].mOffset, props[i].mSize);
				swapbytes(ptr, props[i].mSize);
				ptr += props[i].mSize;
			} else {
				char *aptr = pThis + props[i].mOffset;
				size_t n = props[i].mElementSize;
				for (size_t j = 0; j < props[i].mSize; j = j + n) {
					memcpy(ptr, aptr, n);
					swapbytes(ptr, n);
					aptr += n;
					ptr += n;
				}
			}
		}
	}

	/**
	* @brief byte_swap for each element in  property map does a byte swap, even
	* if
	* array.
	*/
	void byte_swap() {
		PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();
		char *pThis = (char *)this;
		for (size_t i = 0; props[i].mName != NULL; i++)
			if (!props[i].bIsArray) {
				swapbytes(pThis + props[i].mOffset, props[i].mSize);
			} else {
				char *ptr = pThis + props[i].mOffset;
				size_t n = props[i].mSize / props[i].mElementSize;
				for (size_t j = 0; j < props[i].mElementSize; j++) {
					swapbytes((void *)ptr, n);
					ptr = static_cast<char *>(ptr) + n;
				}
			}
	}

	/**
	* @brief get_value of a property map element, using format specifier
	* to determine how to access the pointer.E.g.,  These format specifiers used:
	* d,x,f
	* are given as %lf=64bit long, %f = 32 bit float %d = 16 bit int, etc.
	* @param p property map row
	* @param ptr pointer  address of variable
	* @return string containing value of property table slot
	*/
	std::string get_value(PropertyMap &p, void *ptr) {
//		boost::shared_lock< boost::shared_mutex > lock(mMutex);
		std::string format = p.mFormat;
		if (format == "%d") {
			int32_t *dptr = (int32_t *)ptr;
			return StrFormat("%d", *dptr);
		} else if (format == "%x") {
			int32_t *dptr = (int32_t *)ptr;
			return StrFormat("0x%x", *dptr);
		} else if (format == "%f") {
			float *fptr = (float *)ptr;
			return StrFormat("%f", *fptr);
		} else if (format == "%lf") {
			double *fptr = (double *)ptr;
			return StrFormat("%f", *fptr);
		} else if (format == "%c") {
			char *fptr = (char *)ptr;
			return StrFormat("%d", (int)*fptr);
		} else if (format == "%s") {
			std::string s((char *)ptr, p.mSize); // maynot have zero terminator
			// char * fptr = (char *) ptr;
			return StrFormat("%s", s.c_str());
		} else if (format == "%llu") {
			uint64_t *fptr = (uint64_t *)ptr;
			return StrFormat("%llu", *fptr);
		}
		return "";
	}

	/**
	* @brief print produces a string of the given structure for each property
	* map element. Use property map formatting string to produce value string.
	* @param prefix
	* @return string of all property map elements concatenated
	*/
	std::string print() {
//		boost::shared_lock< boost::shared_mutex > lock(mMutex);
		if (!bHasData)
			return "";
		PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();
		char *pThis = (char *)this;
		std::string tmp;
		tmp += StrFormat("%s\n", static_cast<T *>(this)->msgname());
		for (size_t i = 0; props[i].mName != NULL; i++) {
			tmp += StrFormat("\t%s = ", props[i].mName);
			if (!props[i].bIsArray) {
				tmp += get_value(props[i], pThis + props[i].mOffset);
			} else {
				bool bVector = false;
				// Write an array of simple values
				// m contains array size
				size_t m = props[i].mSize;

				char *ptr = pThis + props[i].mOffset;

				size_t n = props[i].mElementSize; // actually size of element

				if (props[i].mArraySize != NULL) {
					// This is actual size of array, not max amount
					// Should check less that allocated array
					m = (*props[i].mArraySize) * props[i].mElementSize;
					bVector = true;
				}
				for (size_t j = 0; j < m; j = j + n) {
					tmp += get_value(props[i], ptr);
					tmp += ":";
					ptr += n;
				}
			}
			tmp += "\n";
		}
		return tmp;
	}
	/**
	* @brief dump produces a string describing the property map
	* @return string
	*/
	std::string dump() {
		PropertyMap *props = static_cast<T *>(this)->GetPropertyMap();
		std::string tmp;
		for (size_t i = 0; props[i].mName != NULL; i++) {
			tmp += StrFormat("Name = %s Addr=0x%d Size = %d\n", props[i].mName,
				props[i].mOffset, props[i].mSize);
		}
		return tmp;
	}
};
#pragma pop_macro("SCOPED_LOCK")

#endif // MACROS_H
