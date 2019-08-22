#pragma once

enum SeekModes {
	FROM_START, FROM_CURR_POS, FROM_END
};

enum BufferStatus {
	OUT_OF_BOUNDS_READ = -3, FULL, PROTECTED, END_OF_FILE, OK
};

/*@TODO:

*/

class StickyBuffer {
private:
	char* m_buf = nullptr;
	size_t m_size;
	//when used as a circular buffer, the m_used index is used to keep track of the last inserted element. It does not represent the amount of data stored since it wraps around.
	size_t m_used;
	char* m_internal_pointer = nullptr;
	bool m_circular;
	bool m_protected;

	size_t abs(int x) {
		return x >= 0 ? x : -x;
	}

public:
	StickyBuffer() {
		m_buf = nullptr;
		m_size = 0;
		m_used = 0;
		m_circular = false;
		m_internal_pointer = nullptr;
		m_protected = false;
	}

	StickyBuffer(size_t size, bool circular = false, bool _protected = false)
		:m_size(size), m_used(0), m_circular(circular), m_protected(_protected)
	{
		if (m_size) {
			m_buf = new char[m_size];
			m_internal_pointer = m_buf;
			//trivial memset
			for (; m_internal_pointer < m_buf + m_size; ++m_internal_pointer) {
				*m_internal_pointer = 0;
			}
			m_internal_pointer = m_buf;
		}
		else {
			m_buf = nullptr;
			m_internal_pointer = nullptr;
		}
	}
	StickyBuffer(const StickyBuffer& buf)
		:m_size(buf.getSize()), m_used(buf.getBytesUsed()), m_circular(buf.isCircular()), m_protected(buf.isProtected())
	{
		m_buf = new char[m_size];
		memcpy(m_buf, buf.getCharArray(), m_size);
		m_internal_pointer = m_buf;
	}
	StickyBuffer(StickyBuffer&& buf)
		:m_size(buf.getSize()), m_used(buf.getBytesUsed()), m_circular(buf.isCircular()), m_protected(buf.isProtected())
	{
		m_buf = buf.getCharArray();
		buf.nullify();
		m_internal_pointer = m_buf;
	}

	StickyBuffer(void* src, size_t size, bool _copy = true, bool _circ = false, bool _prot = false)
		: m_size(size), m_circular(_circ), m_protected(_prot)
	{
		if (_copy)
		{
			m_buf = new char[m_size];
			memcpy(m_buf, src, m_size);
			m_internal_pointer = m_buf;
		}
		else {
			m_buf = static_cast<char*>(src);
			src = nullptr;
			m_internal_pointer = m_buf;
		}

		int i = 0;
		for (; i < m_size; ++i) {
			if (!m_buf[i])break;
		}
		m_used = i;
	}

	~StickyBuffer() {
		if (m_buf) {
			delete[] m_buf;
			m_size = 0;
			m_used = 0;
			m_internal_pointer = nullptr;
			m_circular = false;
		}
	}

	size_t getSize() const {
		return m_size;
	}

	size_t getBytesFree() const {
		return m_size - getBytesUsed();
	}

	size_t getBytesUsed() const {
		return m_used;
	}

	bool isCircular() const {
		return m_circular;
	}

	bool isProtected() const {
		return m_protected;
	}

	void setProtected(bool cond) {
		m_protected = cond;
	}

	char* getCharArray() const {
		return (m_buf);
	}

	void nullify() {
		m_buf = nullptr;
		m_internal_pointer = nullptr;
		m_size = 0;
		m_circular = false;
		m_protected = false;
		m_used = 0;
	}

	int seek(int pos, SeekModes mode = SeekModes::FROM_START) {
		if (!m_size || !m_buf)throw "Unallocated buffer";
		switch (mode)
		{
		case SeekModes::FROM_START:
			//circular seek
			if (pos >= 0)
				m_internal_pointer = m_buf + pos % m_size;
			else {
				m_internal_pointer = m_buf + abs((m_size + pos) % (int)m_size);
			}
			return 1;
		case SeekModes::FROM_CURR_POS:

			if (m_internal_pointer + pos >= m_buf + m_size || m_internal_pointer + pos < m_buf) {
				return -1;
			}
			m_internal_pointer += pos;
			return 1;
		case SeekModes::FROM_END:
			if (pos <= 0)
				m_internal_pointer = m_buf + (m_size - 1 + pos) % m_size;
			else
				m_internal_pointer = m_buf + m_size - 1 + (pos % (int)m_size);
			return 1;
		}
	}

	template <class T>
	BufferStatus get(T& placeHolder) {
		if (m_internal_pointer == m_buf + m_size) {
			if (m_circular) {
				m_internal_pointer = m_buf;
			}
			else {
				return END_OF_FILE;
			}
		}
		if (m_internal_pointer + sizeof(T) > m_buf + m_size)return OUT_OF_BOUNDS_READ;
		placeHolder = *(T*)m_internal_pointer;
		size_t temp = sizeof(T);
		m_internal_pointer += sizeof(T);

		return OK;
	}
	template <class T>
	BufferStatus add(T val) {
		if (m_protected)return PROTECTED;
		if (m_circular) {
			*((T*)(m_buf + m_used % m_size)) = val;
			return OK;
		}
		else {
			if (m_used + sizeof(T) > m_size)return FULL;
			*((T*)(m_buf + m_used)) = val;
			m_used += sizeof(T);
			return OK;
		}
	}

	std::string to_string() const {
		std::string res;
		for (register int i = 0; i < m_size; ++i) {
			if (m_buf[i])res += (m_buf[i]);
		}
		return res;
	}
};