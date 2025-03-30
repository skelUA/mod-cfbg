CREATE TABLE `cross_faction_battlegrounds` (
    `guid` int unsigned NOT NULL,
    `enabled` tinyint unsigned NOT NULL DEFAULT '1',
    PRIMARY KEY(`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;