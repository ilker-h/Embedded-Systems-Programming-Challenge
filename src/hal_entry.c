/* HAL-only entry function */
#include "hal_data.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define COUNTS_PER_MILLISECOND (120E6 / 1000)
bsp_leds_t leds;

void hal_entry(void)
{
    int s4 = 0; // button s4 for Green Player
    int s5 = 0; // button s5 for Amber Player
    // Turn Meanings:
    // 0 = green player's turn, 1 = amber player's turn, 2 = delay after green player's turn, 3 = red light after green player's turn
    // 4 = delay after amber player's turn, 5 = red light after amber player's turn
    int turn;
    const int POINTS_TO_WIN = 10;

    time_t t;
    timer_size_t counts = 0;
    int random;

    srand((unsigned) time(&t));
    turn = rand() % 2;
    random = (rand()%(400))+100;
    ioport_level_t player1;
    ioport_level_t player2;
    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

    //creating timer
    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg); //creating timer
    g_timer0.p_api->reset(g_timer0.p_ctrl);

    // start the game with a random delay in which all LEDs are off
    while (counts < random*COUNTS_PER_MILLISECOND){
        g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off
        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

        // either player can lose multiple points if they keep clicking during the delay
        if (player1 == IOPORT_LEVEL_LOW){
            g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
            g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//            s4--;
        }
        if (player2 == IOPORT_LEVEL_LOW){
            g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
            g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//            s5--;
        }
    }

    g_timer0.p_api->reset(g_timer0.p_ctrl); // this resets the timer


    while(s4 < POINTS_TO_WIN && s5 < POINTS_TO_WIN){

        // green player's turn
        while (turn == 0) {

                g_timer0.p_api->reset(g_timer0.p_ctrl);

                // turn on green LED for 300ms
                while (counts <= 700*COUNTS_PER_MILLISECOND && turn == 0){
                    g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_LOW); //turns green LED on
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off

                    // the amber player loses points if they click on green player's turn
                    if (player2 == IOPORT_LEVEL_LOW && turn == 0) {
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
                        // s5--; // losing points is the only thing we could not get to work
                    }

                    // if green player clicks button during their turn, they get a point then their turn ends
                    if (player1 == IOPORT_LEVEL_LOW && turn == 0) {
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
                        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off

                        g_timer0.p_api->reset(g_timer0.p_ctrl);
                        s4++;
                        turn = 2; // this starts a random delay

                     //turn++;  //to hand it off to amber player // this may cause the while loop to end before the other code below it runs
                    }

                    // if the green player did not click the button on time, they lose a point and the red LED turns on
                    g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                    if (counts >= 300*COUNTS_PER_MILLISECOND) {
                        turn = 3; // goes to red LED's if statement
                        g_timer0.p_api->reset(g_timer0.p_ctrl);
                    }
                }
            }


        // if the green player did click on time
        if (turn == 2) {

            //generate random number for delay time and start the delay
            random = (rand()%(400))+100;
            g_timer0.p_api->reset(g_timer0.p_ctrl);
            g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts); //new

            while (counts < random*COUNTS_PER_MILLISECOND){
                g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                // either player can lose multiple points if they keep clicking during the delay
                if (player1 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s4--; // losing points is the only thing we could not get to work
                }
                if (player2 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s5--; // losing points is the only thing we could not get to work
                }
            }
            turn = 1; // pass off to amber player
        }


        // if the green player did not click on time
        if (turn == 3) {
            g_timer0.p_api->reset(g_timer0.p_ctrl);

            while (counts <= 500*COUNTS_PER_MILLISECOND && turn == 3){
                g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_LOW); //turns red LED on
                g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                // either player can lose multiple points if they keep clicking during the red light
                if (player1 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s4--; // losing points is the only thing we could not get to work
                }
                if (player2 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s5--; // losing points is the only thing we could not get to work
                }

                // once the timer for the red LED is done, make a random delay, then make it the amber player's turn
                g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                if (counts >= 500*COUNTS_PER_MILLISECOND){
                    turn = 2; // goes to the random delay
                    g_timer0.p_api->reset(g_timer0.p_ctrl);
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off
                }
            }
        }


        // for amber player
        while (turn == 1) {
                g_timer0.p_api->reset(g_timer0.p_ctrl);

                // turn on amber LED for 300ms
                while (counts <= 700*COUNTS_PER_MILLISECOND && turn == 1){
                    g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);

                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_LOW); //turns amber LED on

                    // the green player loses points if they click on green player's turn
                    if (player1 == IOPORT_LEVEL_LOW && turn == 1) {
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                        s4--; // losing points is the only thing we could not get to work
                    }

                    // if amber player clicks button during their turn, they get a point then their turn ends
                    if (player2 == IOPORT_LEVEL_LOW && turn == 1) {
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                        g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
                        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off

                        s5++;
                        turn = 4; // pass off to delay

                     g_timer0.p_api->reset(g_timer0.p_ctrl);
                    }

                    // if the amber player did not click the button on time, they lose a point and the red LED turns on
                    g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                    if (counts >= 300*COUNTS_PER_MILLISECOND) {
                        turn = 5;
                        g_timer0.p_api->reset(g_timer0.p_ctrl);
                    }
                }
            }


        // if the amber player did click on time
        if (turn == 4) {

            //generate random number for delay time and start the delay
            random = (rand()%(400))+100;
            g_timer0.p_api->reset(g_timer0.p_ctrl);
            g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts); //new

            while (counts < random*COUNTS_PER_MILLISECOND){
                g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                // either player can lose multiple points if they keep clicking during the delay
                if (player1 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s4--; // losing points is the only thing we could not get to work
                }
                if (player2 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s5--; // losing points is the only thing we could not get to work
                }
            }
            turn = 0; // pass off to green player
        }


        // if the amber player did not click on time
        if (turn == 5) {
            g_timer0.p_api->reset(g_timer0.p_ctrl);

            while (counts <= 500*COUNTS_PER_MILLISECOND && turn == 5){
                g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_LOW); //turns red LED on
                g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);

                // either player can lose multiple points if they keep clicking during the red light
                if (player1 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s4--; // losing points is the only thing we could not get to work
                }
                if (player2 == IOPORT_LEVEL_LOW){
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, &player1);
                    g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_05, &player2);
//                    s5--; // losing points is the only thing we could not get to work
                }

                // once the timer for the red LED is done, make a random delay, then make it the green player's turn
                g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
                if (counts >= 500*COUNTS_PER_MILLISECOND){
                    turn = 4; // goes to the random delay
                    g_timer0.p_api->reset(g_timer0.p_ctrl);
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
                    g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off
                }
            }
        }
    }


    // once 10 points has been reached, keep the winner's light on forever
    if (s4 >= POINTS_TO_WIN){
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_LOW); //turns green LED on
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH); //turns amber LED off
    }
    else if (s5 >= POINTS_TO_WIN) {
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH); //turns green LED off
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH); //turns red LED off
        g_ioport.p_api -> pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_LOW); //turns amber LED on
    }

}
