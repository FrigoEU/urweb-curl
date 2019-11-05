val get : url -> option string (* auth header *) -> option string (* user/pwd *) -> transaction string
val post : url -> option string (* auth header *) -> option string (* user/pwd *) -> option string (* body Content-type *) -> string (* body *) -> transaction string
val put : url -> option string (* auth header *) -> option string (* user/pwd *) -> option string (* body Content-type *) -> string (* body *) -> transaction string
val delete : url -> option string (* auth header *) -> option string (* user/pwd *) -> transaction string
