#ifndef CEVENT_H
#define CEVENT_H

#include <QQueue>
#include <QMutex>

class Event   {
    public:
        Event() {}
        Event(const Event &other) 
        {
            dir = other.dir;
            name = other.name;
            desc = other.desc;
            exits = other.exits;
            blind = other.blind;
            scout = other.scout;
            terrain = other.terrain;
            prompt = other.prompt;
            movement = other.movement;
        }

        Event &operator=(const Event &other) 
        {
            if (this != &other) {  // make sure not same object
                dir = other.dir;
                name = other.name;
                desc = other.desc;
                exits = other.exits;
                blind = other.blind;
                scout = other.scout;
                terrain = other.terrain;
                prompt = other.prompt;
                movement = other.movement;
            }
            return *this;    // Return ref for multiple assignment            
        }

        void clear()    {
            dir = "";
            name = "";
            desc = "";
            exits = "";
            prompt = "";
            scout = false;
            blind = false;
            movement = false;
            terrain = -1;
        }

        QByteArray dir;
        QByteArray name;
        QByteArray desc;
        QByteArray exits;
        bool       blind;         /* fog, no light, blind flag */
        bool       scout;
        bool       movement;    
        char       terrain;
        QByteArray prompt;
};

class PipeManager {
    QMutex pipeMutex;
    QQueue<Event> Pipe;

public:
    void addEvent(Event e)  { pipeMutex.lock(); Pipe.enqueue(e); pipeMutex.unlock();  }
    void clear() { pipeMutex.lock(); Pipe.clear(); pipeMutex.unlock();  }
    bool isEmpty() { return Pipe.empty(); }    

    Event getEvent() 
    {
        Event event;
        pipeMutex.lock(); 
        event = Pipe.dequeue(); 
        pipeMutex.unlock(); 
        return event;
    }; 
};


#endif
