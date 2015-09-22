CREATE TABLE auth
(`id` INTEGER PRIMARY KEY,
 `login` VARCHAR(8),
 `rg_username` VARCHAR(32),
 `rg_time` TIMESTAMP,
 `key` VARCHAR(32));

CREATE TABLE log
(`id` INTEGER PRIMARY KEY,
 `auth_id` INTEGER,
 `repo_id` INTEGER,
 `time` TIMESTAMP,
 `mark` INTEGER);

CREATE TABLE repositories
(`id` INTEGER PRIMARY KEY,
 `repo` VARCHAR(32));
