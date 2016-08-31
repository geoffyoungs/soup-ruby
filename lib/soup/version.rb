module Soup
  BINDINGS_VERSION= [0,0,10]
  def BINDINGS_VERSION.to_s
    join('.')
  end
end
