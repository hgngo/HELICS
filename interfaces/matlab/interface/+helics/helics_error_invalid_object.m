function v = helics_error_invalid_object()
  persistent vInitialized;
  if isempty(vInitialized)
    vInitialized = helicsMEX(0, 55);
  end
  v = vInitialized;
end