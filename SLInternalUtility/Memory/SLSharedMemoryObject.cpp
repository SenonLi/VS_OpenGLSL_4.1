#include "../stdafx.h"
#include "SLSharedMemoryObject.h"

#include "emmintrin.h" // for SSE aligned memory allocation/deallocation

namespace slutil
{
	//============================================================================================================
	//--------------------------- SLAlignedMemoryBuffer -----------------------------------------------------------------

	int SLAlignedMemoryBuffer::m_InstanceCounter = 0; // Declaration of private static counter

	SLAlignedMemoryBuffer::SLAlignedMemoryBuffer(unsigned long long totalBytes, unsigned int alignment)
	{
		AllocateAlignedMemory(totalBytes, alignment);
	}

	SLAlignedMemoryBuffer::~SLAlignedMemoryBuffer()
	{
		if (m_BufferEntry != nullptr) 
		{
			_mm_free(m_BufferEntry);
			m_InstanceCounter--;

			assert(m_InstanceCounter >= 0);
			std::wstringstream outputMessage;
			outputMessage << _T("SLAlignedMemoryBuffer::~SLAlignedMemoryBuffer(), There exist now \t") << m_InstanceCounter << _T(" \t SLAlignedMemoryBuffer allocated.\n");
			SLOutputDebugString(outputMessage.str().c_str());
		}
	}

	/// <summary> allocate Aligned Memory </summary>
	void SLAlignedMemoryBuffer::AllocateAlignedMemory(unsigned long long totalBytes, unsigned int alignment)
	{
		assert(totalBytes > 0 && m_BufferEntry == nullptr);

		// Make sure m_TotalBytes > 0 when doing an actual allocation
		m_InstanceCounter++;

		// It won't hurt to allocate a bit more, and will help a lot preventing potential crash
		unsigned int alignedMemoryAllocationSafetyOffset = alignment * 4;
		m_BufferEntry = static_cast<BYTE*>(_mm_malloc(totalBytes + alignedMemoryAllocationSafetyOffset, alignment));

		std::wstringstream outputMessage;
		outputMessage << _T("SLAlignedMemoryBuffer::AllocateAlignedMemory(), There exist now \t")
			<< m_InstanceCounter << _T(" \t SLAlignedMemoryBuffer allocated.\n");
		SLOutputDebugString(outputMessage.str().c_str());
	}

	//--------------------------- SLAlignedMemoryBuffer -----------------------------------------------------------------
	//============================================================================================================
	//--------------------------- SLSharedMemoryObject -----------------------------------------------------------------
	SLSharedMemoryObject::SLSharedMemoryObject()
	{
		m_SharedBuffer = std::make_shared<SLAlignedMemoryBuffer>();
	}

	SLSharedMemoryObject::SLSharedMemoryObject(unsigned long long totalBytes, unsigned int alignment)
	{
		CreateSharedMemory(totalBytes, alignment);
	}

	/// <summary> Will Create directly, no need to call Reset 
	///           Will automatically re-new m_SharedBuffer and m_TotalBytes </summary>
	void SLSharedMemoryObject::CreateSharedMemory(unsigned long long totalBytes, unsigned int alignment)
	{
		m_TotalBytes = totalBytes;
		m_SharedBuffer = std::make_shared<SLAlignedMemoryBuffer>(m_TotalBytes, alignment);
	}

	/// <summary>Reset SLSharedMemoryObject</summary>
	void SLSharedMemoryObject::Reset()
	{
		m_SharedBuffer.reset();
		m_TotalBytes = 0;
	}


} // End of namespace slutil