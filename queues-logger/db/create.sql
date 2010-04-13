-- create database
CREATE table "queue_info" ("id" INTEGER PRIMARY KEY, "call_time_t" int, "queue_name" text, "caller" text, "caller_uniqueid" text, "call_picker" text, "hold_time" int, "talk_time" int);
CREATE INDEX queue_info_call_time_t_index ON "queue_info" ("call_time_t");
CREATE INDEX queue_info_queue_name_index ON "queue_info" ("queue_name");
