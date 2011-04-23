-- CREATE DATABASE karia2_resource;
DROP TABLE IF EXISTS meta_links;
CREATE TABLE meta_links (
       uid BIGSERIAL,
       url VARCHAR(3000) NOT NULL,
       file_name VARCHAR(128) NOT NULL,
       file_name_md5  VARCHAR(32) NOT NULL,
       file_content_md5 VARCHAR(32),
       file_content_digest_md5 VARCHAR(32),
       ed2k_hash VARCHAR(32),
       use_level INTEGER NOT NULL,
       valid_flag INTEGER NOT NULL,
       supply_user_name VARCHAR(60),
       ctime TIMESTAMP NOT NULL,
       mtime TIMESTAMP,
       gid BIGINT,
       PRIMARY  KEY (uid),
       UNIQUE (url)
);

DROP TABLE IF EXISTS group_links;
CREATE TABLE group_links (
       gid BIGSERIAL,
       link_count INTEGER NOT NULL,
       PRIMARY KEY (gid)
);

DROP TABLE IF EXISTS mirror_hosts;
CREATE TABLE mirror_hosts (
       hid SERIAL,
       host_name VARCHAR(128) NOT NULL,
       mid INTEGER,
       use_level INTEGER NOT NULL,
       valid_flag INTEGER NOT NULL,
       PRIMARY KEY (hid),
       UNIQUE (host_name)
);

DROP TABLE IF EXISTS skype_gateways;
CREATE TABLE skype_gateways (
       skype_id varchar(100) not null,
       area_type integer not null default 0,
       in_use integer not null default 0,
       rand_key integer not null default 0,
       lock_time timestamp,
       caller_name varchar(100),
       callee_phone varchar(100),
       ws_port integer not null default 0,
       ws_ipaddr varchar(50),
       PRIMARY KEY (skype_id)
);
CREATE INDEX on skype_gateways(caller_name);

DROP TABLE IF EXISTS skype_callpairs;
CREATE TABLE skype_callpairs (
       skype_id varchar(100) not null,
       callee_phone varchar(100) not null,
       caller_ipaddr varchar(32),
       lock_time timestamp,
       delete_flag integer not null default 0,
       PRIMARY KEY (skype_id)
);

DROP FUNCTION merge_replace_skype_callpairs (text, text, text);
CREATE OR REPLACE FUNCTION merge_replace_skype_call_pairs(p_skype_id TEXT, p_callee_phone TEXT, p_caller_ipaddr TEXT) RETURNS VOID AS
$$
BEGIN
    LOOP
        -- first try to update the key
        UPDATE skype_callpairs SET callee_phone = p_callee_phone,lock_time=NOW(), delete_flag=0, 
                                   caller_ipaddr = p_caller_ipaddr
                WHERE skype_id = p_skype_id;
        IF found THEN
            RETURN;
        END IF;
        -- not there, so try to insert the key
        -- if someone else inserts the same key concurrently,
        -- we could get a unique-key failure
        BEGIN
            INSERT INTO skype_callpairs(skype_id, callee_phone, caller_ipaddr,
                                        lock_time, delete_flag)
                    VALUES (p_skype_id, p_callee_phone, p_caller_ipaddr, NOW(), 0);
            RETURN;
        EXCEPTION WHEN unique_violation THEN
            -- do nothing, and loop to try the UPDATE again
        END;
    END LOOP;
END;
$$
LANGUAGE plpgsql;

DROP TABLE IF EXISTS skype_routers;
CREATE TABLE skype_routers (
       skype_id varchar(100) not null,
       in_service integer not null default 0,
       PRIMARY KEY (skype_id)
);
INSERT INTO skype_routers VALUES ('liuguangzhao01', 1);
