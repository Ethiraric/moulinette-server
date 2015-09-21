CREATE TABLE auth
(`id` INTEGER PRIMARY KEY,
 `login` VARCHAR(8),
 `key` VARCHAR(32));

CREATE TABLE log
(`id` INTEGER PRIMARY KEY,
 `auth_id` INTEGER,
 `time` TIMESTAMP,
 `mark` INTEGER
 `user_output` TEXT);

CREATE TABLE subject
(`id` INTEGER PRIMARY KEY,
 `day` DATE);

CREATE TABLE mark
(`id` INTEGER PRIMRAY KEY,
 `auth_id` INTEGER,
 `subject_id` INTEGER);
