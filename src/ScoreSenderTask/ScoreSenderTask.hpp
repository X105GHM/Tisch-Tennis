#ifndef SCORE_SENDER_TASK_HPP
#define SCORE_SENDER_TASK_HPP

class ScoreSenderTask 
{
public:
    static void startTask(void *parameter);
    static void setDisplayActive(bool active);
    static bool getDisplayActive();

private:

    static volatile bool isDisplayActive;
};

#endif // SCORE_SENDER_TASK_HPP
