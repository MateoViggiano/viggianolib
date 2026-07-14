#pragma once
#include "../metafunctions/metafunctions.hpp"
#include "../Classes/Stacks_Queues.hpp"
#include "../Classes/AutoPtrv2/PointerBase.hpp"
namespace mpv{
    template<typename StateType>
    class StateMachine{

        template<typename T,typename=void> struct has_enter:false_type{};
        template<typename T> struct has_enter<T,void_t<decltype(declval<T>().enter())>>:true_type{};
        template<typename T,typename=void> struct has_exit:false_type{};
        template<typename T> struct has_exit<T,void_t<decltype(declval<T>().exit())>>:true_type{};
        static_assert(has_enter<StateType>::value,"StateType must have a enter() method");
        static_assert(has_exit<StateType>::value,"StateType must have a exit() method");
        enum class Action:char{NONE,PUSH,POP,REPLACE,POP_UNTILL_FIND} incoming_action=Action::NONE;
        Stack<uPtr<StateType>> stack;
        uPtr<StateType> incoming_state;
        bool(*func)(StateType*);
        template<typename Derived>
        static bool is(StateType* p){
            return (bool)dynamic_cast<Derived*>(p);
        }        
    public:
        void push(StateType* new_state){
            incoming_state=new_state;
            incoming_action=Action::PUSH;
        }
        void pop(){
            incoming_action=Action::POP;
        }
        void replace_current(StateType* new_state){
            incoming_state=new_state;
            incoming_action=Action::REPLACE;
        }

        template<typename Derived>
        void pop_until_find(){
            func = &is<Derived>;
            incoming_action = Action::POP_UNTILL_FIND;
        }
        void process_change(){
            switch(incoming_action){
                case Action::NONE:break;
                case Action::PUSH:
                    if(!stack.empty())
                        stack.top()->exit();
                    stack.push(move(incoming_state));
                    stack.top()->enter();
                    break;
                case Action::REPLACE:
                    if(stack.empty()){
                        stack.push(move(incoming_state));
                    }
                    else{
                        stack.top()->exit();
                        stack.top()=move(incoming_state);
                    }
                    stack.top()->enter();
                    break;
                case Action::POP:
                    if(!stack.empty()){
                        stack.top()->exit();
                        stack.del();
                        if(!stack.empty())
                            stack.top()->enter();
                    }break;
                case Action::POP_UNTILL_FIND:
                    if(!stack.empty() && !func(stack.top().get())){
                        stack.top()->exit();
                        stack.del();
                    }
                    while(!stack.empty() && !func(stack.top().get())){
                        stack.del();
                    }
                    if(!stack.empty())
                        stack.top()->enter();
            }
            incoming_action=Action::NONE;
        }
        StateType& current(){
            return *stack.top();
        }
        StateType* get_current(){
            return stack.top().get();
        }
        bool empty()const{
            return stack.empty();
        }
    };
}
