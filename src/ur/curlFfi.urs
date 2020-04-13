val get : url -> option string (* auth header *) -> option string (* user/pwd *) -> transaction string
val post : url -> option string (* auth header *) -> option string (* user/pwd *) -> option string (* body Content-type *) -> string (* body *) -> transaction string
val put : url -> option string (* auth header *) -> option string (* user/pwd *) -> option string (* body Content-type *) -> string (* body *) -> transaction string
val delete : url -> option string (* auth header *) -> option string (* user/pwd *) -> transaction string

type curl
val mkCurl:
    option string (* verb *) ->
    transaction curl
val setBodyString: curl -> string -> transaction curl
val setBodyBlob: curl -> blob -> transaction curl
val addHeader:
    curl ->
    string ->
    string ->
    transaction curl
val setUserPwd:
    curl ->
    string ->
    transaction unit
type result
val run:
    curl ->
    url ->
    transaction result
val getHttpCode: result -> int
val getResult: result -> string
