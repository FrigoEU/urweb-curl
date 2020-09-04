fun buildQueryParams
  (params: (list (string * string))) : string =
  case params of
    [] => ""
  | first :: rest =>
    List.foldl
      (fn tup acc => acc ^ "&" ^ tup.1 ^ "=" ^ Urls.urlencode tup.2)
      (first.1 ^ "=" ^ Urls.urlencode first.2)
      rest

(* TODO return real error instead of packaging in fake status code? *)
fun handleResult str =
    case String.split str #";" of
      None => { StatusCode = 998
              , Response = Some ("Error: Failed to deserialize result from CURL, didn't find \";\" : " ^ str) }
    | Some splitted =>
      case (read splitted.1: option int) of
        None => { StatusCode = 999
                , Response = Some ("Error: Failed to deserialize result from CURL, couldn't read status code as int : " ^ str) }
      | Some code => { StatusCode = code
                     , Response = if String.trim splitted.2 = "" then None else Some splitted.2
                     }

fun postWithFormdata
      (url: url)
      (queryParams: list (string * string))
      (authheader: option string)
      (userpwd: option string)
    : transaction {StatusCode: int, Response: option string} =
    Monad.mp handleResult
             (CurlFfi.post url authheader userpwd None (buildQueryParams queryParams))

fun getWithUrlEncoding
      (url: url)
      (queryParams: list (string * string))
      (authheader: option string)
      (userpwd: option string)
    : transaction {StatusCode: int, Response: option string} =
    Monad.mp
      handleResult
      (CurlFfi.get
         (bless ((show url) ^ "?" ^ buildQueryParams queryParams))
         authheader
         userpwd)

fun get (url: url) : transaction {StatusCode: int, Response: option string} =
    Monad.mp handleResult (CurlFfi.get url None None)
