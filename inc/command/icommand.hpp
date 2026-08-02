#pragma once

namespace command {

    class ICommand {

        public:
            virtual ~ICommand() = default;

            virtual void Execute() = 0;
            virtual void Commit() = 0;
            virtual void Undo() = 0;
            
    };

}