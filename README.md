# rlite-php

[![Build Status](https://travis-ci.org/seppo0010/rlite-php.svg?branch=master)](https://travis-ci.org/seppo0010/rlite-php)

PHP bindings for rlite. For more information about rlite, go to
[rlite repository](https://github.com/seppo0010/rlite)

## Installation

Before installing rlite-php, you'll need to install rlite

```sh
git clone https://github.com/seppo0010/rlite.git
cd rlite
make && sudo make install
```

Now you can continue with the extension set up.

```sh
git clone https://github.com/seppo0010/rlite-php.git
cd phpiredis
phpize && ./configure --enable-rlite
make && sudo make install
```

## Usage

```php
$rlite = create_rlite_connection(':memory:');
rlite_command($rlite, ['RPUSH', 'test', '1', '2', '3']);
var_dump(rlite_command($rlite, ['LRANGE', 'test', '0', '-1']));
/*
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
*/
```

### Persistence

```php
$rlite = create_rlite_connection('./mydb.rld');
rlite_command($rlite, ['SET', 'key', 'value']);

$rlite = create_rlite_connection('./mydb.rld');
var_dump(rlite_command($rlite, ['GET', 'key']));
/*
string(5) "value"
*/
```
