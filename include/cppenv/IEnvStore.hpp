#pragma once
// store value before parsing env file 


namespace cppenv {

    class IEnvStore {
        public:

            // set value in storage  
            virtual void set_value() = 0;
            
            // get value in storage 
            virtual void get() = 0;


    };


}