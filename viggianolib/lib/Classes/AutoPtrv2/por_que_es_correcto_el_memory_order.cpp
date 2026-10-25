#include<iostream>
#include<atomic>
#include<assert.h>
#include<thread>
#include"../../../viggiano"
using namespace std;
using namespace mpv;
struct Struct{
	testObj x=0;
	~Struct(){
		assert(x==20);
	}
};
int main(){// En caso de que t2 termine primero la logica es trivial.
    //El razonamiento va a ser suponiendo que t1 termina primero
	sPtr<Struct> sptr1(new Struct);
	sPtr<Struct> sptr2(sptr1);
	thread t1([p=std::move(sptr1)]()mutable{
		p->x=20; // no se actualiza en memoria hasta que sharedCount se decrementa.
		p=nullptr;// se decrementa sharedCount. memory_order_release actualiza en memoria las escrituras previas (p->x=20)
		
	});
	thread t2([p=std::move(sptr2)]()mutable{
		p=nullptr;// se decrementa sharedCount. memory_order_release actualiza en memoria las escrituras previas (en este caso no las hay)
        // al ver que sharedCount es 0, memory_order_acquire adquiere las modificaciones actualizadas en memoria (p->x=20) y llama a destroy_object() con los valores actualizados
	});//                                ^(en el thread fence) 
	/*
    - si el decremento no fuese memory_order_release, el memory_order_acquire del atomic_thread_fence no
        adquiriria los cambios que se hicieron en los otros threads antes de cada decremento
    - si atomic_thread_fence(memory_order_acquire) no estuviese, al llamar a destroy_object() despues de un
        decremento de sharedCount a 0, no se tendria el objeto actualizado por los release de los decrementos de otros threads
    */ 
    t1.join();
	t2.join();
}