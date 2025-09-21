template <typename T, int Capacity, typename Comparator>
class SortedBuffer {
public:
    SortedBuffer(Comparator cmp = Comparator()) : cmp_(cmp), size_(0) {}

    bool push(const T& item) {
        if (size_ >= Capacity) return false;
        int i = size_;
        while (i > 0 && cmp_(buffer_[i-1], item)) {
            buffer_[i] = buffer_[i-1];
            i--;
        }
        buffer_[i] = item;
        size_++;
        return true;
    }

    bool pop(T& out) {
    if (size_ == 0) return false;
    out = buffer_[0]; // Get the first (smallest/largest, depending on cmp_) item
    // Shift all elements down
    for (int i = 1; i < size_; ++i) {
        buffer_[i - 1] = buffer_[i];
    }
    size_--;
    return true;
}

    const T* peek() const { return size_ ? &buffer_[0] : nullptr; }
    int size() const { return size_; }

    void clear() { size_ = 0; }

    bool isFull() const { return size_ >= Capacity; }


private:
    T buffer_[Capacity];
    int size_;
    Comparator cmp_;
};


//Usage:
/*
struct PlayTimeCmp {
    bool operator()(const MidiEvent& a, const MidiEvent& b) const {
        return a.playTime > b.playTime;
    }
};
SortedBuffer<MidiEvent, 128, PlayTimeCmp> delayedNotes;
*/