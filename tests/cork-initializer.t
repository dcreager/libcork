We need to sort the output, since there's no guarantee about which order our
initializer functions will run in.

  $ cork-initializer | sort
  Finalizer 1
  Initializer 1
  Initializer 2
