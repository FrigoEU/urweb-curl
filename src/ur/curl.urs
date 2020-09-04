val buildQueryParams: list (string * string) -> string

val postWithFormdata : url ->
                       list (string * string) ->
                       option string -> (* Auth header *)
                       option string -> (* user / pwd *)
                       transaction { StatusCode: int
                                   , Response: option string}

val getWithUrlEncoding : url ->
                         list (string * string) ->
                         option string -> (* Auth header *)
                         option string -> (* user / pwd *)
                         transaction { StatusCode: int
                                     , Response: option string}

val get : url -> transaction { StatusCode: int
                             , Response: option string}
