// Exercise the escape sequences across the same state diff used on the wire.
#include <string>
#include "completeterminal.h"
#include "fatal_assert.h"

using namespace Terminal;

int main()
{
  Complete server( 40, 2 ), client( 40, 2 );
  server.act( "\033[1;2;38;2;13;117;209mA\033[22mB\033]52;;Y29weQ==\007" );
  client.apply_string( server.diff_from( client ) );

  const Renditions &faint = client.get_fb().get_cell( 0, 0 )->get_renditions();
  const Renditions &normal = client.get_fb().get_cell( 0, 1 )->get_renditions();
  fatal_assert( faint.get_attribute( Renditions::faint ) && faint.get_attribute( Renditions::bold ) );
  fatal_assert( !normal.get_attribute( Renditions::faint ) && !normal.get_attribute( Renditions::bold ) );
  fatal_assert( normal.sgr().find( "38;2;13;117;209" ) != std::string::npos );
  const Framebuffer::title_type clipboard = client.get_fb().get_clipboard();
  fatal_assert( std::wstring( clipboard.begin(), clipboard.end() ) == L"Y29weQ==" );

  // Neither a read request nor an unrelated selection is a clipboard write.
  server.act( "\033]52;;?\007\033]52;p;b3RoZXI=\007" );
  client.apply_string( server.diff_from( client ) );
  fatal_assert( client.get_fb().get_clipboard() == clipboard );

  server.act( "\033]52;c;\007" );
  client.apply_string( server.diff_from( client ) );
  fatal_assert( client.get_fb().get_clipboard().empty() );
}
