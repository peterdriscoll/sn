import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';

// Root component
import { AppComponent } from './app.component';

// Services
import { HttpClientModule } from '@angular/common/http';

// UI components
import { HeaderComponent } from './header/header.component';
import { ThreadTabsComponent } from './thread-tabs/thread-tabs.component';
import { ToolbarComponent } from './toolbar/toolbar.component';
import { BreakpointsComponent } from './breakpoints/breakpoints.component';
import { StatusComponent } from './status/status.component';
import { ErrorsComponent } from './errors/errors.component';
import { WorldSetsComponent } from './world-sets/world-sets.component';
import { DelayedCallsComponent } from './delayed-calls/delayed-calls.component';
import { WatchListComponent } from './watch-list/watch-list.component';
import { CallStackComponent } from './call-stack/call-stack.component';
import { StackComponent } from './stack/stack.component';
import { CommandBreakpointsComponent } from './command-breakpoints/command-breakpoints.component';
import { CodeLogComponent } from './code-log/code-log.component';
import { DerivationComponent } from './derivation/derivation.component';
import { SimpleLogComponent } from './simple-log/simple-log.component';
import { CssValidComponent } from './css-valid/css-valid.component';
import { ExplorerComponent } from './explorer/explorer.component';

// Pipes
import { TrustAsHtmlPipe } from './pipes/trust-as-html.pipe';

@NgModule({
  declarations: [
    AppComponent,
    HeaderComponent,
    ThreadTabsComponent,
    ToolbarComponent,
    BreakpointsComponent,
    StatusComponent,
    ErrorsComponent,
    WorldSetsComponent,
    DelayedCallsComponent,
    WatchListComponent,
    CallStackComponent,
    StackComponent,
    CommandBreakpointsComponent,
    CodeLogComponent,
    DerivationComponent,
    SimpleLogComponent,
    CssValidComponent,
    ExplorerComponent,
    TrustAsHtmlPipe
  ],
  imports: [
    BrowserModule,
    FormsModule,
    CommonModule,
    HttpClientModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
