
#if defined _MemoryEx_BaseMemory_include_
  #endinput
#endif
#define _MemoryEx_BaseMemory_include_


#include <sdktools>

#define Pointer Address
#define nullptr Address_Null

#define PTR(%0) view_as<Pointer>(%0)

enum
{
	MemoryEx_NoNeedAdd = (1 << 0),

	/*
		When will read from memory is started, MemoryEx will automatically add to this.pAddrBase. 

		...
		this.SetAddr(0x200) =>
		int byte  = this.ReadWord(0x20) [Read something]
		Now this.GetAddr return 0x222 => 0x200 [Base] + 0x20 [offset] + 0x02 [size type]
		...
	*/
	MemoryEx_AddAfterRead = (1 << 1),
	MemoryEx_AddAfterWrite = (1 << 2),
}

enum struct BaseMemory
{
    Pointer pAddrBase;

    void SetAddr(any address)
	{
		if(address == nullptr) 
		{
			return;
		}

		this.pAddrBase = PTR(address);
	}
	Pointer GetAddr()
	{
		return this.pAddrBase;
	}
    void Add(any iOffset)
	{
		this.pAddrBase += iOffset;
	}

	int ReadByte(int iOffset = 0)
	{
		return LoadFromAddress(this.pAddrBase + PTR(iOffset), NumberType_Int8);
	}
	void WriteByte(any iByte, int iOffset = 0,  int flags = MemoryEx_NoNeedAdd)
	{
		Pointer addr = this.GetAddr() + PTR(iOffset);

		StoreToAddress(addr, iByte, NumberType_Int8);
		
		if(flags & MemoryEx_AddAfterWrite)
		{
			this.Add(0x01);
		}
	}
	
	int ReadWord(int iOffset = 0)
	{
		return LoadFromAddress(this.pAddrBase + PTR(iOffset), NumberType_Int16);
	}
	void WriteWord(any iWord, int iOffset = 0,  int flags = MemoryEx_NoNeedAdd)
	{
		Pointer addr = this.GetAddr() + PTR(iOffset);

		StoreToAddress(addr, iWord, NumberType_Int16);

		if(flags & MemoryEx_AddAfterWrite)
		{
			this.Add(0x02);
		}
	}
	
	int ReadInt	(int iOffset = 0)
	{
		return LoadFromAddress(this.pAddrBase + PTR(iOffset), NumberType_Int32);
	}
	void WriteInt(any iNumber, int iOffset = 0, int flags = MemoryEx_NoNeedAdd)
	{
		Pointer addr = this.GetAddr() + PTR(iOffset);

		StoreToAddress(addr, iNumber, NumberType_Int32);

		if(flags & MemoryEx_AddAfterWrite)
		{
			this.Add(0x04);
		}
	}
	void WriteData(const int[] data, int iSize, int flags = MemoryEx_NoNeedAdd)
	{
		int x;

		for(x = 0; x < iSize; x++)
		{
			this.WriteByte(data[x], x);
		}

		if(flags & MemoryEx_AddAfterWrite)
		{
			this.Add(x);
		}
	}
	int ReadString(char[] sString, int iMaxLength)
	{
		int x;
		int iByte;

		for(x = 0; x < iMaxLength; x++)
		{
			iByte = this.ReadByte(x);

			if(iByte == 0x0)
			{
				break;
			}

			sString[x] = iByte;
		}

		return x;
	}
	void WriteString(const char[] sString, bool bNull = true, int flags = MemoryEx_NoNeedAdd)
	{
		int x;
		for(x = 0; x < strlen(sString) ; x++)
		{
			this.WriteByte(sString[x], x);
		}

		if(bNull)
		{
			this.WriteByte(0x00, x);
		}

		if(flags & MemoryEx_AddAfterWrite)
		{
			this.Add(x);
		}
	}
	void WriteUnicodeString(const char[] sString, bool bNull = true, int flags = MemoryEx_NoNeedAdd)
	{
		int x;
		int index;
        
		for(x = 0; x <= (strlen(sString) - 1) * 2; x += 2)
		{
			this.WriteWord(sString[index], x);
			index++;
		}

		if(bNull)
		{
			this.WriteWord(0x00, x);
		}

		if(flags & MemoryEx_AddAfterWrite)
		{
			this.Add(x + 0x02);
		}
	}
	Pointer FindPattern(Pointer base, any size, const int[] pattern, int iLength, int iOffset = 0)
	{
		Pointer addr; // default = nullptr
		Pointer end = base + size;

		int iByte;
		bool found;

		while(base < end)
		{
			iByte = LoadFromAddress(base, NumberType_Int8);

			if(iByte == pattern[0])
			{
				found = true;

				for(int x = 1; x < iLength; x++)
				{
					if(pattern[x] != 0x2A)
					{ 
						iByte = LoadFromAddress(base + PTR(x), NumberType_Int8);

						if(iByte != pattern[x])
						{
							found = false;
							break;
						}
					}
				}

				if(found)
				{
					addr = base + PTR(iOffset);
					break;
				}
			}

			base++;
		}

		return addr;
	}
}