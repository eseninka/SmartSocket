create table calculation
(
num_measurements serial not null,
uuid text               not null,
time                    timestamp default current_timestamp,
P                       real not null,
I                       real not null,
V                       real not null,
cosfi                   real not null
);

create table data_current
(
num_measurements serial not null,
uuid text               not null,
time                    timestamp default current_timestamp,
flash_current           text not null,
rms_current             real not null
);

create table data_voltage
(
num_measurements serial not null,
uuid text               not null,
time                    timestamp default current_timestamp,
flash_voltage           text not null,
rms_voltage             real not null
);

create table data_power_records
(
num_records serial      primary key,
uuid text               not null,
date_records            DATE DEFAULT CURRENT_DATE,
data_records            real not null
);