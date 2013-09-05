module Soup
  BINDINGS_VERSION= [0,0,7]
  def BINDINGS_VERSION.to_s
    join('.')
  end
end
