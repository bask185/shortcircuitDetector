#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"

// #define DEBUG

const int MAX_CURRENT = 100 ;       // almost 1A
const int MAX_FLANK   = 100 ;       // to be Deteremened

uint16_t    prevSample ;
uint8_t     counter ;
uint8_t     sensitivity ;
uint16_t    sample ;
uint16_t    dI ;
uint16_t    dIprev ;
uint32_t    interval ;
bool        restart ;

void setup()
{
    initIO() ;

    #ifdef DEBUG
    Serial.begin( 115200 ) ;
    Serial.println( version ) ;
    Serial.println( date ) ;
    #endif

    sample = analogRead( adjust ) ;
    sensitivity = map ( sample, 0, 1023, 10, 50 ) ;
    interval = 1 ;
}

void loop()
{
    REPEAT_MS( interval )
    {
        // IF POWER IS OFF
        if( READ( PORTB, 1 ) == 0 )                                         
        {
            SET( PORTB, 1 ) ;                                                   // RE-ENABLE POWER
            CLR( PORTB, 0 ) ;                                                   // CLEAR LED
            restart = true ;
            interval = 1 ;
            #ifdef DEBUG
            Serial.println("power ON" ) ;
            #endif
        }

        // IF POWER IS ON
        else 
        {
            sample = analogRead( currentSens ) ;

            dI = abs( sample - prevSample ) ;                                   // calculate how fast overcurrent rose
            prevSample = sample ;

            if( dI != dIprev )
            {
                dIprev = dI ;
                #ifdef DEBUG
                Serial.write('I') ;
                Serial.println(dI) ;
                #endif
                goto shortcircuit ;
            }

            if( restart == true )                                               // IF shortcircuit is still present after restarting, immediately kill the power again
            {   restart = false ;
                
                if( sample >= MAX_CURRENT ) goto shortcircuit ; 
            }

            if( sample <= MAX_CURRENT ) counter = sensitivity ;                 // if no over current, keep setting the counter to 'sensitivity'

            if( dI >= MAX_FLANK || -- counter == 0 )                            // if over current lasted longer than 'sensitivity'ms OR the flank was too high, kill power
            {
            shortcircuit:
                CLR( PORTB, 1 ) ;                                               // KILL POWER
                SET( PORTB, 0 ) ;                                               // SET LED
                interval = 1000 ;                                               // WAIT
                #ifdef DEBUG
                Serial.println("ALARM!!!!!") ;
                #endif
            }
        }

    }   END_REPEAT

}