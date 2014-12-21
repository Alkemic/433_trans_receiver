/*
 * create table readings(created_at DATETIME DEFAULT CURRENT_TIMESTAMP, trans_id INTEGER, value TEXT, type TEXT);
 */
#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>

#define RX_PIN 2 //pin number defined by wiringPi
#define DB_NAME "readings.sqlite"

#define DS18B20_SQL "INSERT INTO readings(trans_id, value, type) VALUES(%d, %d, 'DS18B20');"

#define sql_exec(_sql) sqlite3_exec(db, _sql, callback, 0, &zErrMsg)
#define sql_query(_sql, _data) sqlite3_exec(db, _sql, callback, (void*)_data, &zErrMsg)
#define GET_BIT(var, pos) (((var) & (1<<(pos))) >> pos)
#define checksum_correct(value, checksum) (checksum == (GET_BIT(value, 7) << 3) | (GET_BIT(value, 5) << 2) | (GET_BIT(value, 3) << 1) | GET_BIT(value, 1))

char *sql;
RCSwitch receiver;
sqlite3 *db;
sqlite3_stmt * stmt;
char *zErrMsg = 0;
int rc;

uint32_t value;
uint8_t node_id;
uint8_t checksum;

void clean_on_exit(void){
	sqlite3_close(db);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char *argv[]){
    atexit(clean_on_exit);

    if(wiringPiSetup() == -1)
       return 2;

    rc = sqlite3_open(DB_NAME, &db);
    if(rc){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    receiver = RCSwitch();
    receiver.enableReceive(RX_PIN);

    while(1){
        if(receiver.available()){
            value = receiver.getReceivedValue();

            if(value == 0){
                printf("Unknown encoding");
            }else{
                checksum = (value & 0xFF) >> 4;
                node_id = value & 0x0F;
                value = value >> 8;
                if(checksum_correct(value, checksum))
                    printf("Checksum correct\n");
                else
                    printf("Checksum incorrect\n");
                printf("Received %i, %f\n", value, value/16.0);
                sql = sqlite3_mprintf(DS18B20_SQL, node_id, value);
                sqlite3_prepare(db, sql, -1, &stmt, NULL);//preparing the statement
                sqlite3_step(stmt);

                rc = sql_exec(sql);
                if(rc != SQLITE_OK){
                    fprintf(stderr, "SQL error: %s\n", zErrMsg);
                    sqlite3_free(zErrMsg);
                }
                sqlite3_finalize(stmt);
                sqlite3_free(sql);
             }

             receiver.resetAvailable();
        }

        usleep(500000);
    }

    sqlite3_close(db);

    return 0;
}
