#ifndef SystemCall_h
#define SystemCall_h

#include <QProcess>
#include <QString>


class SystemCall : public QProcess {
    /* {src_lang=cpp}*/


public:

    /** simple access function to set the attribute command by function
      * @param value value to set for the attribute command
      */
    void set_command( QString value ) { command = value; };

    /** simple access function to get the attribute output by function */
    QString get_output( void ) const { return output; };

    /** simple access function to set the attribute browser by function
      * @param value value to set for the attribute browser
      */
    void set_browser( QString value ) { browser = value; };

    SystemCall();

    QString execute();

    QString execute(QString command);

    QString executeDetached(QString command);

    void openURL(QString url);

    QString browser;
    /* {set=public}*/

private:
    QString command;
    /* {set=public}*/
    QString output;
    /* {get=public}*/
};

#endif // SystemCall_h
