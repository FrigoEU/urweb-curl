val post :
    url ->
    string -> (* body *)
    option string -> (* authheader *)
    option string -> (* user / pwd *)
    transaction string
val get :
    url ->
    option string -> (* authheader *)
    option string -> (* user / pwd *)
    transaction string
