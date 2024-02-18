
#ifndef ArrayList_H
#define ArrayList_H



template <typename T> class ArrayList  {
public:
     T* elements = new T[10];
    T defaultVal;
    unsigned int size = 0;
    unsigned int totalSize = 10;
 
ArrayList()
{

    /*
    ptr = new T[s];
    size = s;
    for (int i = 0; i < size; i++)
        ptr[i] = arr[i];
        */
}
 
 void add( T value)
{
    resize(size + 1);
    elements[size] = value;
}

 void resize( int newElementPos) {
     if (newElementPos < size) return;
     if (newElementPos >= totalSize ) {
        totalSize = newElementPos + 5;
        T ptr2[newElementPos];

        for (int i = 0; i < newElementPos; i++) {
            if (i < size) ptr2[i] = elements[i];
            else ptr2[i] = defaultVal;
        }
        delete [] elements;

        elements = new T[totalSize];
        size = newElementPos;
        for (int i = 0; i <size; i++) {
            elements[i] = ptr2[i];
        }
    }
 }

    ~ArrayList(){
        delete [] elements;
    }

    T& operator[](int index)
    {
        resize(index);
        if (index >= size) size = index + 1;
        return elements[index];
    }

    void operator=(ArrayList<T>& a1) 
    { 
        delete [] elements; 
        totalSize = a1.totalSize;
        size = a1.size;
        elements = new T[totalSize];
        //Serial.println(String("size: ") + size );

        for (int i = 0; i <size; i++) {
           elements[i] = a1[i];
           //Serial.println(String("el") + i + ": " + a1[i] );
        }
    } 



};
#endif


