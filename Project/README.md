# Project README

## Compile
In Project root directory
```
make
```

## Execute
In Project root directory
```
./server <port>
```
For example
```
./server 5678
```
Then use your browser `http://localhost:5678`

## API
* Cookies key
  * `HcsUserId`: user id (string)
  * `HcsUserLevel`: user level (integer), 3 levels in total
    * `1`: admin
    * `2`: doctor
    * `3`: patient


* `/login` User login API
  * POST Method
  * Body should be attached
    * `id`: user id
    * `passwd`: user password
  * The API Set-Cookie:
    * `HcsUserId` as level id
    * `HcsUserLevel` as user level
* `/logout`: Logout from current user
  * GET or POST Method
  * The API Set-Cookie:
    * `HcsUserId` as "", empty string
    * Note that, it doesn't change `HcsUserLevel`

## Test example
#### Login
You can test login by browsing `http://localhost:<Port>/index.html`, there is a form to type in username.
For test purpose, three username can be test:
* `testadmin`, with any password, it won't check for now
* `testdoctor`, with any password, it won't check for now
* `testpatient`, with any password, it won't check for now

After submit, check `http://localhost:<port>/user.html`, you'll see the username and user level.

#### Logout
Send request `http://localhost:<port>/logout`, then check `http://localhost:<port>/user.html`, you'll see it shows `User: Not exist`
