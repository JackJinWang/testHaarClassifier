#pragma once
template<typename _Tp, size_t fixed_size = 4096 / sizeof(_Tp) + 8>
class  MyAutoBuffer
{
public:
	typedef _Tp value_type;
	enum { buffer_padding = (int)((16 + sizeof(_Tp) - 1) / sizeof(_Tp)) };

	//! the default contructor  
	MyAutoBuffer();
	//! constructor taking the real buffer size  
	MyAutoBuffer(size_t _size);
	//! destructor. calls deallocate()  
	~MyAutoBuffer();

	//! allocates the new buffer of size _size. if the _size is small enough, stack-allocated buffer is used  
	void allocate(size_t _size);
	//! deallocates the buffer if it was dynamically allocated  
	void deallocate();
	//! returns pointer to the real buffer, stack-allocated or head-allocated  
	operator _Tp* ();
	//! returns read-only pointer to the real buffer, stack-allocated or head-allocated  
	operator const _Tp* () const;

protected:
	//! pointer to the real buffer, can point to buf if the buffer is small enough  
	_Tp* ptr;
	//! size of the real buffer  
	size_t size;
	//! pre-allocated buffer  
	_Tp buf[fixed_size + buffer_padding];
};
/////////////////////////////// MyAutoBuffer ////////////////////////////////////////  

template<typename _Tp, size_t fixed_size> inline MyAutoBuffer<_Tp, fixed_size>::MyAutoBuffer()
{
	ptr = buf;
	size = fixed_size;
}

template<typename _Tp, size_t fixed_size> inline MyAutoBuffer<_Tp, fixed_size>::MyAutoBuffer(size_t _size)
{
	ptr = buf;
	size = fixed_size;
	allocate(_size);
}

template<typename _Tp, size_t fixed_size> inline MyAutoBuffer<_Tp, fixed_size>::~MyAutoBuffer()
{
	deallocate();
}

template<typename _Tp, size_t fixed_size> inline void MyAutoBuffer<_Tp, fixed_size>::allocate(size_t _size)
{
	if (_size <= size)
		return;
	deallocate();
	if (_size > fixed_size)
	{
		ptr = new _Tp[_size];
		size = _size;
	}
}

template<typename _Tp, size_t fixed_size> inline void MyAutoBuffer<_Tp, fixed_size>::deallocate()
{
	if (ptr != buf)
	{
		//cv::deallocate<_Tp>(ptr, size);
		delete[]ptr;
		ptr = buf;
		size = fixed_size;
	}
}

template<typename _Tp, size_t fixed_size> inline MyAutoBuffer<_Tp, fixed_size>::operator _Tp* ()
{
	return ptr;
}

template<typename _Tp, size_t fixed_size> inline MyAutoBuffer<_Tp, fixed_size>::operator const _Tp* () const
{
	return ptr;
}