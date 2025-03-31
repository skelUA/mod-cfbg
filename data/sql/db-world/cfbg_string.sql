DELETE FROM `acore_string` WHERE `entry` IN (50002, 50003, 50004, 50005);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_ruRU`) VALUES
    (50002, "Cross-faction enabled.", "Крос-фракція увімкнена."),
    (50003, "Cross-faction already enabled.", "Крос-фракція вже увімкнена."),
    (50004, "Cross-faction disabled.", "Крос-фракція вимкнена."),
    (50005, "Cross-faction already disabled.", "Крос-фракція вже вимкнена.");