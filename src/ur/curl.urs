val postWithFormdata : url ->
                       list (string * string) ->
                       option string ->
                       transaction { StatusCode: int
                                   , Response: option string}

val getWithUrlEncoding : url ->
                         list (string * string) ->
                         option string ->
                         transaction { StatusCode: int
                                     , Response: option string}
