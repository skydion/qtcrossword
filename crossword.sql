drop database if exists crossword;

create database crossword DEFAULT CHARACTER SET = UTF8;
use crossword;

drop table if exists crosses;
drop table if exists private_data;
drop table if exists grids;
drop table if exists uses;

drop table if exists templates;
drop table if exists words;
drop table if exists questions;

# таблиця з шаблонами решіток
create table templates
(
  _id int unsigned not null auto_increment primary key,
  # кількість рядків в шаблоні
  _rows int not null,

  # кількість совпців в шаблоні
  _columns int not null,

  # картинка для попереднього перегляду існуючих решіток шаблонів
  _preview mediumblob
) engine=innodb;


# таблиця з даними для шаблону решітки
create table grids
(
  _id int unsigned not null auto_increment primary key,

  # покажчик на шаблон в таблиці шаблонів
  _template int unsigned not null,
  index _template_ind(_template),
  foreign key (_template) references templates(_id)
    on delete no action
    on update no action,

  # рядок ячейки в шаблоні
  _row int not null,

  # совпець ячейки в шаблоні
  _column int not null,

  # дані ячейки
  _value int not null
) engine=innodb;


# таблиця зі словами
create table words
(
  _id int unsigned not null auto_increment primary key,
  index _id_ind(_id),

  # слово
  _word char(30) not null default '',

  # використано це слово при складанні кросворду чи ні
  _used boolean not null default false
) engine=innodb;


# таблиця із запитаннями до слів
create table questions
(
  _id int unsigned not null auto_increment primary key,
  index _id_ind(_id),

  # запитання до слова
  _question text not null
) engine=innodb;


# таблиця з даними про кросворд, отримана шляхом сканування шаблону решітки
create table private_data
(
  _id int unsigned not null auto_increment primary key,

  # покажчик на шаблон в таблиці шаблонів
  _template int unsigned not null,
  index _template_ind(_template),
  foreign key (_template) references templates(_id)
    on delete no action
    on update no action,

  # покажчик на слово в таблиці слів
  # _word int unsigned not null,
  # index word_ind(_word),
  # foreign key (_word) references words(_id)
  #  on delete no action
  #  on update no action,

  # покажчик на запитання в таблиці запитань
  # _question int unsigned not null,
  # index question_ind(_question),
  # foreign key (_question) references questions(_id)
  #  on delete no action
  #  on update no action,

  # номер слова
  _numword int unsigned not null,

  # прапорець заповнення шаблону в словi (1-так, 0-нi)
  #_filled boolean not null,

  # рядок слова в кросвордi
  _row int unsigned not null,

  # стовпець слова в кросвордi
  _column int unsigned not null,

  # довжина слова
  _lenght tinyint unsigned not null,

  # наразі нехай буде, але швидше за все це поле не потрібно
  # кiлькiсть перетинiв в словi
  _crosscount tinyint unsigned not null,

  # орiєнтацiя слова в кросвордi (1-вертикальна, 0-горизонтальна)
  _orientation boolean not null
) engine=innodb;


# таблиця про перетини слова
create table crosses
(
  _id int unsigned not null auto_increment primary key,
  _template int unsigned not null,

  # покажчик на таблицю приватних даних про кросворд
  _pd_id int unsigned not null,
  index pd_ind(_pd_id),
  # foreign key (_pd) references private_data(_id)
  #  on delete no action
  #  on update no action,

  # лiнiйна позицiя перетину вiдносно початку слова
  _cpos int unsigned not null,

  # тип перетину
  _ctype tinyint unsigned not null,

  # номер слова яке утворює даний перетин з актуальним
  # словом (пiд актуальним розумiється те слово в якому
  # описується ця структура перетину). Посутi це поле є
  # покажчиком на слово яке перетинає актуальне слово.
  _numword int unsigned not null
) engine=innodb;


# таблиця про використання кросвордів в пресі, інтернеті
create table uses
(
  _id int unsigned not null auto_increment primary key,

  # покажчик на кросвор в таблиці кросвордів
  # _crossword int unsigned not null,
  # index crossword_ind(_crossword),
  # foreign key (_crossword) references crosswords(_id),

  # покажчик на шаблон в таблиці шаблонів
  _template int unsigned not null,
  index template_ind(_template),
  foreign key (_template) references templates(_id),

  # дата друку кросворду
  _date date not null default "0000-00-00",

  # назва видання де використано кросворд
  _name text not null
) engine=innodb;

LOAD DATA INFILE '/home/taras/Projects/qtCrossword/utf8_word.txt' INTO TABLE words (_word);
