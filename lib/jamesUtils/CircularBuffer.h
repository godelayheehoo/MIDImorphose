template <typename T, size_t Capacity>
class CircularBuffer {
public:
    CircularBuffer() : head(0), tail(0), full(false) {}

    bool push(const T& item) {
        buffer[head] = item;
        if (full) tail = (tail + 1) % Capacity;
        head = (head + 1) % Capacity;
        full = head == tail;
        return true;
    }

    bool pop(T& item) {
        if (empty()) return false;
        item = buffer[tail];
        full = false;
        tail = (tail + 1) % Capacity;
        return true;
    }

    bool empty() const { return (!full && head == tail); }
    bool fullBuffer() const { return full; }
    size_t size() const {
        if (full) return Capacity;
        if (head >= tail) return head - tail;
        return Capacity + head - tail;
    }

    T& operator[](size_t index) { return buffer[(tail + index) % Capacity]; }
    const T& operator[](size_t index) const { return buffer[(tail + index) % Capacity]; }

    template <typename Func>
    void removeIf(Func pred) {
        size_t sz = size();
        for (size_t i = 0; i < sz; /*increment inside*/) {
            if (pred((*this)[i])) {
                removeAt(i);
                sz--;
            } else {
                i++;
            }
        }
    }

    bool removeAt(size_t index) {
        size_t sz = size();
        if (index >= sz) return false;

        size_t pos = (tail + index) % Capacity;
        while (pos != head) {
            size_t next = (pos + 1) % Capacity;
            if (next != head) buffer[pos] = buffer[next];
            pos = next;
        }

        head = (head + Capacity - 1) % Capacity;
        full = false;
        return true;
    }

private:
    T buffer[Capacity];
    size_t head;
    size_t tail;
    bool full;
};
