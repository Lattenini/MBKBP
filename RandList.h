#pragma once
#include "Defines.h"
#include <algorithm>
#include <cassert>
class RandList
{
private:
	ui cap;

public:
	int vnum;
	ui *vlist;
	ui *vpos;
	RandList()
	{
		vlist = vpos = nullptr;
	};
	void init(int _cap)
	{
		cap = _cap;
		if (vlist == nullptr)
			vlist = new ui[cap];
		if (vpos == nullptr)
			vpos = new ui[cap];

		vpos = new ui[cap];
		vnum = 0;
		for (ui i = 0; i < cap; i++)
		{
			vpos[i] = cap;
		}
	}
	inline void add(int vid)
	{
		vlist[vnum] = vid;//vlist一个个存放顶点
		vpos[vid] = vnum++;//vpos记录顶点在vlist中的下标
	};
	inline void remove(int vid)
	{
		ui last_id = vlist[vnum - 1];
		ui id_pos = vpos[vid];
		vlist[id_pos] = last_id;
		vpos[last_id] = id_pos;

		--vnum;
		vpos[vid] = cap;
	}
	void clear()
	{
		for (ui i = 0; i < vnum; i++)
			vpos[vlist[i]] = cap;
		vnum = 0;
	}
	ui get(ui i)
	{
		return vlist[i];
	}
	bool contains(int vid)
	{
		return vpos[vid] != cap;
	}
	bool empty() { return vnum == 0; }
	int getSize() { return vnum; }
	ui getCap() { return cap; }
	void dispose()
	{
		if (vlist != nullptr)
		{
			delete[] vlist;
			vlist = nullptr;
		}
		if (vpos != nullptr)
		{
			delete[] vpos;
			vpos = nullptr;
		}
	}
	~RandList()
	{
		dispose();
	}
};

extern int *twoPow;

//用于实现一个位图（BitSet）数据结构:位图是一种用于表示集合的数据结构，其中每个元素都用一个位来表示其是否属于集合。
class MBitSet //主要是用来判断某顶点是否是某顶点的邻居/非邻居
{
private:
	int n, m;
	int cap;
	unsigned *buf;
	const int msk = (1 << 16) - 1;
	int lb(unsigned x)
	{
		if (x & msk)
			return twoPow[x & msk];
		return twoPow[x >> 16 & msk] + 16;
	}

public:
	MBitSet()
	{
		buf = nullptr;
		cap = n = m = 0;
	}
	~MBitSet()
	{
		if (buf != nullptr)
			delete[] buf;
	}
	void allocacte(int _cap)//分配位图的容量，根据指定的 _cap 参数来计算需要多少空间，并在 buf 上分配相应的内存。
	{
		cap = _cap;

		buf = new unsigned[(cap >> 5) + 1];
	}
	void reinit(int _n)//重新初始化位图，将 n 更新为 _n 的高 5 位，m 更新为 _n 的低 5 位，并将 buf 中所有位置为0。
	{
		assert(_n <= cap);
		m = _n & 31;
		n = _n >> 5;
		for (int i = 0; i <= n; ++i)
			buf[i] = 0;
	}
	void flip()//翻转位图中的所有位，将所有1变为0，所有0变为1。
	{
		for (int i = 0; i < n; ++i)
			buf[i] = ~buf[i];
		buf[n] ^= ((unsigned)1 << m) - 1;
	}
	void set(int x) //设置位图中某个位为1，即将索引为 x 的位设置为1。
	{
		buf[x >> 5] ^= (unsigned)1 << (x & 31);
	}
	bool test(int x) //查位图中某个位是否为1，即检查索引为 x 的位是否被设置为1
	{
		return buf[x >> 5] >> (x & 31) & 1;
	}
	int lowbit() //返回位图中值为1的最低位的索引。如果位图为空，则返回-1
	{
		for (int i = 0; i <= n; ++i)
		{
			unsigned x = buf[i] & ((~buf[i]) + 1);
			if (x)
				return lb(x) + (i << 5);
		}
		return -1;
	}
	bool empty()
	{
		for (int i = 0; i <= n; ++i)
			if (buf[i])
				return false;
		return true;
	}
	void operator&=(const MBitSet &rhs)
	{
		for (int i = 0; i <= n; ++i)
			this->buf[i] &= rhs.buf[i];
	}
	void copy(MBitSet &rhs) const
	{
		rhs.n = n;
		rhs.m = m;
		if (rhs.buf == nullptr)
			rhs.buf = new unsigned[n + 1];
		for (int i = 0; i <= n; ++i)
			rhs.buf[i] = buf[i];
	}
};
