//
// Created by summernight on 07.10.23.
//

#ifndef RME_APPLICATION_H
#define RME_APPLICATION_H


class Application {
public:
    virtual void init() = 0;
    virtual void update() = 0;
    bool isInited = false;
};


#endif //RME_APPLICATION_H
