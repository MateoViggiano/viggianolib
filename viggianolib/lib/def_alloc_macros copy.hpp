#define CONSTRUCT_VARARGS(ALLOC,ADDRESS,ARGS)       if constexpr(!Uses_default_construct_v<remove_reference_t<decltype(ALLOC)>,typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type*,decltype(ARGS)...>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::construct(ALLOC,unfancy(ADDRESS),ARGS...);}\
                                                    else{new(ADDRESS) typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type(ARGS...);}//doesn't matter if is trivially constructible or not, uses placement new
//       vvvvv  used to copy/move construct
#define CONSTRUCT(ALLOC,ADDRESS,ARG)                if constexpr(!Uses_default_construct_v<remove_reference_t<decltype(ALLOC)>,typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type*,decltype(ARG)>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::construct(ALLOC,unfancy(ADDRESS),ARG);}\
                                                    else{new(ADDRESS) typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type(ARG);}//doesn't matter if is trivially constructible or not, uses placement new

#define DEFAULT_CONSTRUCT(ALLOC,ADDRESS)            {using val_type=typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type;\
                                                    if constexpr(!Uses_default_construct_v<remove_reference_t<decltype(ALLOC)>,val_type*>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::construct(ALLOC,unfancy(ADDRESS));}\
                                                    else if(!is_trivially_default_constructible_v<val_type>){new(ADDRESS) val_type();}\
                                                    }//else{do nothing}
#define DESTROY(ALLOC,ADDRESS)                      {using val_type=typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type;\
                                                    if constexpr(!Uses_default_destroy_v<remove_reference_t<decltype(ALLOC)>,val_type*>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::destroy(ALLOC,unfancy(ADDRESS));}\
                                                    else if(!is_trivially_destructible_v<val_type>){(ADDRESS)->~val_type();}\
                                                    }//else{do nothing}

#define MEMDEFAULTCONSTRUCT(ALLOC,DEST,SIZE)        if constexpr(!Uses_default_construct_v<AlTy,value_type*,const_reference>){al::memdefaultconstruct(ALLOC,DEST,SIZE);}\
                                                    else if constexpr(!is_trivially_default_constructible_v<value_type>){dflt::memdefaultconstruct(DEST,SIZE);}
                                                    //else{do nothing}
#define MEMFILLCONSTRUCT(ALLOC,DEST,SIZE,FILLWITH)  if constexpr(!Uses_default_construct_v<AlTy,value_type*,const_reference>){al::memfillconstruct(ALLOC,DEST,SIZE,FILLWITH);}\
                                                    else{dflt::memfillconstruct(DEST,SIZE,FILLWITH);} 

#define MEMCOPYCONSTRUCT(ALLOC,DEST,SOURCE,SIZE)    if constexpr(!Uses_default_construct_v<AlTy,value_type*,const_reference>){al::memcopyconstruct(ALLOC,DEST,SOURCE,SIZE);}\
                                                    else{dflt::memcopyconstruct(DEST,SOURCE,SIZE);}

#define MEMMOVECONSTRUCT(ALLOC,DEST,SOURCE,SIZE)    if constexpr(!Uses_default_construct_v<AlTy,value_type*,value_type&&>){al::memmoveconstruct(ALLOC,DEST,SOURCE,SIZE);}\
                                                    else{dflt::memmoveconstruct(DEST,SOURCE,SIZE);}

#define MEMDESTRUCTOR(ALLOC,DEST,SIZE)              if constexpr(!Uses_default_destroy_v<AlTy,value_type*>){al::memdestructor(ALLOC,DEST,SIZE);}\
                                                    else if constexpr(!is_trivially_destructible_v<value_type>){dflt::memdestructor(DEST,SIZE);}
                                                    