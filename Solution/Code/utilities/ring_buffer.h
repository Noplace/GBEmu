/*****************************************************************************************************************
* Copyright (c) 2013 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/

template<typename T>
class RingBuffer {
 public:
  RingBuffer() : counter(0),size_(0) {
    buffer = nullptr;
    Resize(0);
  }
  ~RingBuffer() {
    Resize(0);
  }
  void Resize(int size) {
    if (size != 0) {
      auto new_buffer = new T[size];
      if (buffer != nullptr) {
        memcpy(new_buffer,buffer,this->size_);
        delete []buffer;
      }
      buffer = new_buffer;
    } else {
      if (buffer != nullptr)
        delete [] buffer;
      buffer = nullptr;
    }
    this->size_ = size;
  }
  T& operator [](int index) {
    /*index = index % size;
    //if (index<0)
    index = counter+index;
    while (index<0)
      index += size;*/
    return buffer[index];
  }

  RingBuffer& operator =(T value) {
    buffer[counter++] = value;
    if (counter == size_)
      counter = 0;
    return *this;
  }

  operator T*() {
    return buffer;
  }
  int size() {   return size_;    }
 private:
  T* buffer;
  int counter;
  int size_;
};